--[[
  In order to register a command the following function is used:
    register_command(int KEY: int, MOD mod: int, function: function) 

  Available functions belong to the ed class which is simply a reference to the
  actual text editor and its underlying methods.
  Currently supported functions are the following:
    ed:insert_text("string" : string)
    ed:toggle_palette()
    ed:backspace()
    ed:paste_text()
    ed:new_line()
    ed:tab()

  ed:insert_text() inserts text at the current cursor point
]]

register_command(keys.KEY_H, Mod.CTRL, function(ed)
  ed:insert_text("hello world!")
end)

register_command(keys.KEY_P, Mod.SUPER, function (ed)
  ed:pick_palette(Palette.Magenta)
end)

register_command(keys.KEY_T, Mod.SUPER, function (ed)
  ed:toggle_palette()
end)

register_command(keys.KEY_D, Mod.SUPER, function (ed)
  ed:backspace()
end)

--[[
  Asides from bound functions, there are functions in the global scope that
  get execute at start time.
  Currently supported functions are the following:
    register_command()
    pick_pallete()
]]