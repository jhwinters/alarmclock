#!/usr/bin/env ruby

#
#  A clock to display the current time on a slightly larger touch screen.
#

require 'sdl2'
require 'time'
require 'yaml'

class Despatcher
  #
  #  An object which will poll for SDL2 events and dispatch them according to
  #  requests from other objects.
  #

  class Request
    attr_reader :type, :recipient

    def initialize(type, recipient)
      @type = type
      @recipient = recipient
    end
  end

  def initialize
    @requests = []
  end

  def register(type, recipient)
    # type should be a class of event
    # recipient should be an object with a handle_event() method
    # 
    # This method of storing the registrations is not efficient and
    # can be improved later by being made more structured.
    #
    puts "Registering"
    @requests << Request.new(type, recipient)
  end

  def despatch
    puts "Despatch starting"
    while true
      event = SDL2::Event.poll
      if event
        puts event.inspect
      end
      @requests.each do |request|
        if request.type === event
          request.recipient.handle_event(event)
        end
      end
      sleep(0.1)
    end
  end
end

class Config
  #
  #  A class to load a config file and provide defaults.
  #
  #  I think we should make every effort to fix a broken configuration file
  #  without actually overriding a setting.  Thus:
  #
  #  * Missing settings should be added in
  #  * String keys should be changed to symbols
  #  * Missing sound and font files should be changed to the defaults
  #
  #  Do we then save the updated settings file?  Perhaps after renaming
  #  the old one.
  #
  @@default_file_name = 'config.yaml'
  @@default_config_yaml = <<~DEFAULT_CONFIG
    ---
    :settings:
      :title: "Alarm clock"
      :screen_width: 1024
      :screen_height: 600
      :alarm_sound_file: Alarm_Classic.ogg
      :dim_delay: 60
      :bright:    200
      :dim:       30
      :fonts:
        :large:
          :file: '/usr/share/fonts/truetype/freefont/FreeSerifBoldItalic.ttf'
          :size: 240
        :medium:
          :file: '/usr/share/fonts/truetype/freefont/FreeSerif.ttf'
          :size: 50
        :small:
          :file: '/usr/share/fonts/truetype/freefont/FreeSans.ttf'
          :size: 32
    :alarms:
      - :time: 05:50
        :days: [Monday]
      - :time: 06:00
        :days: [Monday, Tuesday]
  DEFAULT_CONFIG

  #
  #  Note that there is no context for each of these.  Thus names
  #  need to be unique to provide the context.
  #
  @@Constraints = {
    title:            :a_string,
    settings:         :a_hash,
    screen_width:     :an_integer,
    screen_height:    :an_integer,
    alarm_sound_file: :an_existing_file,
    fonts:            :a_hash,
    large:            :a_hash,
    medium:           :a_hash,
    small:            :a_hash,
    file:             :an_existing_file,
    size:             :an_integer,
    alarms:           :an_array_of_alarms,
    time:             :a_time,
    days:             :an_array,
    dim_delay:        :an_integer,
    bright:           :an_integer,
    dim:              :an_integer
  }

  def interpret_days(list)
    #
    #  Try to interpret an array of strings as days of the week and return
    #  an array of size 7 containing those mentioned.
    #
    #  [true, true, false, true, false, false, false]
    #
    #  That sort of thing.  If we can't interpret them we return nil.
    #
    result = [false, false, false, false, false, false, false]
    list.each do |item|
      case item
      when /Sun/
        result[0] = true
      when /Mon/
        result[1] = true
      when /Tue/
        result[2] = true
      when /Wed/
        result[3] = true
      when /Thu/
        result[4] = true
      when /Fri/
        result[5] = true
      when /Sat/
        result[6] = true
      else
        return nil
      end
    end
    return result
  end

  def validate_as(type, proposed, default)
    if proposed.kind_of?(type)
      return proposed, false
    else
      return default, true
    end
  end

  def validate_integer(proposed, default)
    validate_as(Integer, proposed, default)
  end

  def validate_string(proposed, default)
    validate_as(String, proposed, default)
  end

  def validate_existing_file(proposed, default)
    if proposed.kind_of?(String) &&
        File.exist?(proposed)
      return proposed, false
    else
      return default, true
    end
  end

  def validate_alarm(proposed)
    #
    #  Each alarm needs a time and an optional specification of days.
    #
    time = proposed[:time]
    result = time.kind_of?(Integer) && time >= 0 && time <= 86400
    if result
      #
      #  Do we have a day specification?  Not having one is fine, but if we
      #  have one then it must be valid.
      #
      days = proposed[:days]
      if days
        if days.kind_of?(Array)
          result = (interpret_days(days) != nil)
        else
          result = false
        end
      end
    end
    result
  end

  def validate_array_of_alarms(proposed, default)
    if proposed.kind_of?(Array)
      #
      #  This one gets more interesting and we need to work in a slightly
      #  different way because we can't be driven entirely by our default
      #  data.  The array provided in the proposed configuration may have
      #  any number of members.
      #
      #  Only if it fails to be an array at all do we use our default.
      #
      result = Array.new
      modified = false
      proposed.each do |entry|
        if validate_alarm(entry)
          result << entry
        else
          modified = true
        end
      end
      return result, modified
    else
      return default, true
    end
  end

  def validate_hash(proposed, default)
    #
    #  Validate a proposed hash and return a validated version, making
    #  use of as much of the proposed hash as possible, with substitutions
    #  from the defaults where the proposed section was not acceptable.
    #
    if proposed.kind_of?(Hash)
      result = Hash.new
      modified = false
      default.each do |key, value|
        constraint = @@Constraints[key]
        case constraint
        when :a_string
          result[key], was_modified = validate_string(proposed[key], value)
          if was_modified
            modified = true
          end
        when :a_hash
          result[key], was_modified = validate_hash(proposed[key], value)
          if was_modified
            modified = true
          end
        when :an_integer
          result[key], was_modified = validate_integer(proposed[key], value)
          if was_modified
            modified = true
          end
        when :an_existing_file
          result[key], was_modified =
            validate_existing_file(proposed[key], value)
          if was_modified
            modified = true
          end
        when :an_array_of_alarms
          result[key], was_modified =
            validate_array_of_alarms(proposed[key], value)
          if was_modified
            modified = true
          end
        #when :a_time
        else
          #
          #  Use the default
          #
          result[key] = value
          modified = true
        end
      end
    else
      #
      #  It isn't a hash at all so we'll have to use the defaults.
      #
      result = default
      modified = true
    end
    return result, modified
  end

  def validate(proposed_config)
    default_config = YAML.load(@@default_config_yaml)
    config, modified = validate_hash(proposed_config, default_config)
  end

  def save(file_name = @@default_file_name)
    if File.exist?(file_name)
      save_name = "#{file_name}.org"
      if File.exist?(save_name)
        File.delete(save_name)
      end
      File.rename(file_name, save_name)
    end
    File.open(file_name, 'w') do |f|
      YAML.dump(@details, f)
    end
  end

  def initialize(file_name = @@default_file_name)
    #
    #  Try first for a settings file on disk.
    #
    proposed_details = nil
    begin
      proposed_details = YAML.load_file(file_name)
      puts proposed_details.inspect
    rescue Errno::ENOENT
      puts "No existing config file."
    end
    @details, modified = validate(proposed_details)
    #puts @details.inspect
    if modified
      save()
      puts "Config file created/modified."
    end
    #
    #  Now set up some instance variables with necessary information
    #  from our validated configuration.
    #
    @screen_width  = @details[:settings][:screen_width]
    @screen_height = @details[:settings][:screen_height]
    @title         = @details[:settings][:title]
    @dim_delay     = @details[:settings][:dim_delay].to_f
  end

  #
  #  And now methods for the running program to query the configuration.
  #
  attr_reader :screen_width, :screen_height, :title, :dim_delay

end


#
#  I propose to wrap up the SDL stuff a bit.  Other things apart from
#  my clock class might want to write to it.
#
class MyDisplay

  #
  #  I propose to provide methods to write text to the screen.  You can
  #  specify a reference point from which to start, then an offset from
  #  that reference.
  #
  #  If you specify left reference the offset will be for the left of
  #  the text.
  #
  #  If you specify right reference the offset will be for the right of
  #  the text.
  #
  #  If you specify centre reference then the offset will be for the
  #  centre of the text.
  #
  #  In other words, how far to move from touching the left edge, touching
  #  the right edge or being centred.
  #
  #  Likewise for vertical.
  #
  #  Sadly, plain Ruby doesn't have an enum type (although Rails adds one).
  #
  #enum horizontal_reference: [:left, :centre, :right]
  #enum vertical_reference: [:top, :middle, :bottom]
  #

  #
  #  Curiously, you don't need any kind of handle on the display in order
  #  to write to it.  This is used purely to get dimensions.
  #
  attr_reader :width, :height

  def initialize(config)
    SDL2.init(SDL2::INIT_EVERYTHING)
    SDL2::TTF.init
    @display = SDL2::Display.displays[0]
    @width = @display.current_mode.w
    @height = @display.current_mode.h
    puts "Initial width and height #{@width}, #{@height}"
    SDL2::Mouse::Cursor.hide
    @window = SDL2::Window.create(
      config.title,
      SDL2::Window::POS_CENTERED,
      SDL2::Window::POS_CENTERED,
      config.screen_width,
      config.screen_height,
      SDL2::Window::Flags::FULLSCREEN)        # Flags
    @renderer = @window.create_renderer(
      -1,       # Index
      0)        # Flags
    @width = @display.current_mode.w
    @height = @display.current_mode.h
    puts "Final width and height #{@width}, #{@height}"

#    while SDL2::Mixer::Channels.play?(0)
#      sleep 1
#    end
  end

  def blank_buffer
    @renderer.draw_color = [0,0,0]
    @renderer.fill_rect(
      SDL2::Rect.new(0,0,@width,@height))
  end

  def paint_text(
    text,
    font,
    href,
    vref,
    hoff,
    voff,
    density)

    text_width, text_height = font.size_text(text)
    case href
    when :left
      hpos = hoff
    when :right
      hpos = (@width - text_width) - hoff
    when :centre
      hpos = (@width - text_width) / 2 + hoff
    end
    case vref
    when :top
      vpos = voff
    when :bottom
      vpos = (@height - text_height) - voff
    when :middle
      vpos = (@height - text_height) / 2 + voff
    end
    surface = font.render_solid(text, [density, density, density])
    texture = @renderer.create_texture_from(surface)
    @renderer.copy(
      texture,
      nil,
      SDL2::Rect.new(hpos, vpos, text_width, text_height)
    )
    texture.destroy
    surface.destroy
  end

  def do_display
    @renderer.present
  end

end

class AlarmClock

  ITALIC_FONT_FILE = '/usr/share/fonts/truetype/freefont/FreeSerifBoldItalic.ttf'
  PLAIN_FONT_FILE = '/usr/share/fonts/truetype/freefont/FreeSerif.ttf'

  SMALL_FONT_FILE = '/usr/share/fonts/truetype/freefont/FreeSans.ttf'

  ITALIC_FONT_SIZE = 240
  PLAIN_FONT_SIZE = 50
  SMALL_FONT_SIZE = 32

  def initialize(config, my_display, despatcher)
    #
    #  First we want to know how big our display is.
    #
    @screen_width = my_display.width
    @screen_height = my_display.height
    @my_display = my_display
    @config     = config

    @italic_font = SDL2::TTF.open(ITALIC_FONT_FILE, ITALIC_FONT_SIZE)
    @plain_font  = SDL2::TTF.open(PLAIN_FONT_FILE, PLAIN_FONT_SIZE)
    @small_font  = SDL2::TTF.open(SMALL_FONT_FILE, SMALL_FONT_SIZE)

    #
    #  Whether we're showing a night-friendly dim version of the time.
    #
    @faded = false
    @last_touched_time = Time.now

    SDL2::Mixer.init(SDL2::Mixer::INIT_OGG)
    SDL2::Mixer.open(22050, SDL2::Mixer::DEFAULT_FORMAT, 2, 512)
    @alarm_sound = SDL2::Mixer::Chunk.load("Alarm_Classic.ogg")
    SDL2::Mixer::Channels.set_volume(0, 128)
    SDL2::Mixer::Channels.fade_in(0, @alarm_sound, 0, 600)
    @sounding = true
    #
    #  What events do we need?
    #
    despatcher.register(nil, self)  # For now until timers implemented
    despatcher.register(SDL2::Event::FingerDown, self)
    despatcher.register(SDL2::Event::KeyDown, self)

  end

  def ordinalize(number)
    case number
    when 11, 12, 13
      suffix = "th"
    else
      case number % 10
      when 1
        suffix = "st"
      when 2
        suffix = "nd"
      when 3
        suffix = "rd"
      else
        suffix = "th"
      end
    end
    "#{number}#{suffix}"
  end

  def handle_event(event)
    if event
      puts "In handle_event"
      puts event.inspect
    end
    case event
    when SDL2::Event::FingerDown
      @last_touched_time = Time.now
      @faded = false
    when SDL2::Event::KeyDown
      if event.sym == 113   # 'q'
        exit
      end
    end
    t = Time.now()
    if t - @last_touched_time > @config.dim_delay
      @faded = true
    end
    time_string = t.strftime("%H:%M")
    date_string = t.strftime("#{ordinalize(t.day)} %B, %Y")
    @my_display.blank_buffer
    @my_display.paint_text(
      time_string,
      @italic_font,
      :centre,
      :middle,
      0,
      @faded ? 0 : -30,
      @faded ? 30 : 200)
    unless @faded
      @my_display.paint_text(
        date_string,
        @plain_font,
        :centre,
        :middle,
        0,
        100,
        200)
      @my_display.paint_text(
        "Top left",
        @small_font,
        :left,
        :top,
        0,
        0,
        200)
      @my_display.paint_text(
        "Top right",
        @small_font,
        :right,
        :top,
        0,
        0,
        200)
      @my_display.paint_text(
        "Bottom left",
        @small_font,
        :left,
        :bottom,
        0,
        0,
        200)
      @my_display.paint_text(
        "Bottom right",
        @small_font,
        :right,
        :bottom,
        0,
        0,
        200)
    end
    @my_display.do_display
    if @sounding
      unless SDL2::Mixer::Channels.play?(0)
        SDL2::Mixer.close
        @sounding = false
      end
    end
  end

end

#
#  Start of main code
#
puts "Getting config"
config = Config.new
puts "Creating despatcher"
despatcher = Despatcher.new
puts "Initalising display"
my_display = MyDisplay.new(config)
puts "Creating alarm clock"
alarm_clock = AlarmClock.new(config, my_display, despatcher)
puts "Starting despatcher"
#despatcher.despatch()

#
#  Should not get here.
#
iterations = 0
while true
  event = SDL2::Event.poll
  if event
    puts event.inspect
  end
  alarm_clock.handle_event(event)
  sleep(0.1)
  iterations += 1
  if iterations > 1000
    GC.start
    iterations = 0
  end
end


