#!/usr/bin/env ruby

#
#  Just blank the screen.
#

require 'sdl2'

SDL2.init(SDL2::INIT_EVERYTHING)

window = SDL2::Window.create(
  "Alarm Clock",
  SDL2::Window::POS_CENTERED,
  SDL2::Window::POS_CENTERED,
  720,     # Width
  576,      # Height
  0)        # Flags

renderer = window.create_renderer(
  -1,       # Index
  0)        # Flags


renderer.draw_color = [0,0,0]
renderer.fill_rect(
  SDL2::Rect.new(0,0,720,576))
renderer.present
SDL2::Mouse::Cursor.hide
while true
  sleep(5.0)
end

