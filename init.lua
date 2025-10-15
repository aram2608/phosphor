editor.register_command("hello.world", function()
	editor.hello_world()
end)
editor.bind_key(editor.keys.KEY_LEFT_SHIFT, "hello.world")