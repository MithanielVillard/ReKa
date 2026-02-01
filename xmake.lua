
add_rules("mode.debug", "mode.release")

set_languages("c++20")

if is_mode("debug") then
	add_defines("DEBUG")
end

includes("AAL")
includes("Engine")

target("App")
	add_files("main.cpp")
	add_deps("ReKa")
	add_deps("ReKaGraph")
