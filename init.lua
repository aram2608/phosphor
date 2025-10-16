--[[
  In order to register a command, 
  the register_command(int KEY: int, MOD mod: int, function: function) 
  format should be used.

  Available functions belong to the ed class which is simply a reference to the
  actual text editor and its underlying methods.
  Currently supported functions are the following:
    ed:insert_text("string" : string)

  ed:insert_text() inserts text at the current cursor point
]]

register_command(keys.KEY_H, Mod.CTRL, function(ed)
  ed:insert_text("Saved!\n")
end)