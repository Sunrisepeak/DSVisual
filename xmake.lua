add_rules("mode.debug", "mode.release")

set_project("dsvisual")

set_languages("cxx17")

-- config third-party libs

---- imgui
add_requires("imgui 1.88", {configs = {glfw_opengl3 = true}})
add_packages("imgui")

---- DStruct: data struct library
add_includedirs("third-party/DStruct")

---- hanim: animate framework
add_includedirs("third-party/Hanim")

---- XRecorder: OpenGLRecorder
add_includedirs("third-party/XRecorder")
if is_host("linux") then
    add_includedirs("/usr/include/opencv4")
    add_links("opencv_core")
    add_links("opencv_highgui")
    add_links("opencv_imgproc", "opencv_videoio", "opencv_imgcodecs")
end

-- imgui node
add_includedirs("third-party/imnodes")
add_files("third-party/imnodes/imnodes.cpp")

-- config dsvisual
add_files("dsv-core/dsvisual-core.cpp")
add_includedirs(".")

target("dsvisual_array")
    set_kind("binary")
    add_files("examples/ds/array.cpp")

target("dsvisual_embedded_list")
    set_kind("binary")
    add_files("examples/ds/embedded_list.cpp")

target("dsvisual_bubble_sort_visual")
    set_kind("binary")
    add_files("examples/bubble_sort_visual.cpp")