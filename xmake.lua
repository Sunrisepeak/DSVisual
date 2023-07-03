set_languages("c++17")

-- libs
add_requires("imgui 1.88", {configs = {glfw_opengl3 = true}})

add_includedirs(
    ".",
    "third-party/DStruct"
)

add_packages("imgui")

--[[
target("dsvisual")
    set_kind("shared")
    add_files("dsvisual.cpp")
--]]

target("dsvisual_array")
    set_kind("binary")
    add_cxxflags("-g")
    add_files("examples/array.cpp")