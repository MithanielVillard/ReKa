target("ReKaGraph")
	set_kind("shared")
	add_includedirs("include", {public = true})
    add_files("src/*.cpp")
	add_deps("ReKa")

