set_project("cpp_mindflow")
set_languages("c++23")
add_requires("decimal_for_cpp")
-- Превращаем предупреждения в ошибки (strict сборка)
set_warnings("all", "error")

-- Безопасные политики Xmake (включают ASAN для пакетов и жесткий контроль)
set_policy("build.sanitizer.address", true) -- Правильное имя для ASan политики!
set_policy("build.warning", true)

target("tool")
    set_kind("binary")
    add_files("src/*.cpp")
    add_packages("decimal_for_cpp") 