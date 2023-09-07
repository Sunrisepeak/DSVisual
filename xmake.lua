add_rules("mode.debug", "mode.release")

set_project("dsvisual")

set_languages("cxx17")

-- config third-party libs

---- imgui
add_requires("imgui 1.88", {configs = {glfw_opengl3 = true}})
add_packages("imgui")

-- imgui node
add_includedirs("third-party/imnodes")
add_files("third-party/imnodes/imnodes.cpp")

---- DStruct: data struct library
add_includedirs("third-party/DStruct")

---- Hanim: animate framework
add_includedirs("third-party/Hanim")

---- XRecorder: OpenGLRecorder
add_includedirs("third-party/XRecorder")
if is_host("linux") then
    add_includedirs("/usr/include/opencv4")
    add_links("opencv_core")
    add_links("opencv_highgui")
    add_links("opencv_imgproc", "opencv_videoio", "opencv_imgcodecs")
end

-- config dsvisual
add_includedirs(".")
add_files("dsv-core/dsvisual-core.cpp")

target("dsvisual_array")
    set_kind("binary")
    add_files("examples/ds/array.cpp")

target("dsvisual_embedded_list")
    set_kind("binary")
    add_files("examples/ds/embedded_list.cpp")

target("dsvisual_sort-visualization")
    set_kind("binary")
    add_files("examples/algo/sort-visualization.cpp")

target("dsvisual_sort-bugview")
    set_kind("binary")
    add_files("examples/algo/sort-bugview.cpp")