add_rules("mode.debug", "mode.release")

set_project("dsvisual")

set_languages("cxx17")

-- config third-party libs
add_requires("imgui 1.88", {configs = {glfw_opengl3 = true}})
add_packages("imgui")
add_includedirs("third-party/DStruct") -- Only-Header

-- config dsvisual
add_files("dsv-core/dsvisual-core.cpp")
add_includedirs(".")

target("dsvisual_array")
    set_kind("binary")
    add_files("examples/ds/array.cpp")

target("dsvisual_bubble_sort_visual")
    set_kind("binary")
    add_files("examples/bubble_sort_visual.cpp")