set CONAN_EXE=conan.exe

set BUILD_TYPE=Release

%CONAN_EXE% profile detect --force 

@REM %CONAN_EXE% install ./win32_conanfile.txt --build=missing  --settings:all=compiler.cppstd=17 --settings:all=build_type=%BUILD_TYPE%
%CONAN_EXE% install ./conanfile.py --build=missing  --settings:all=compiler.cppstd=17 --settings:all=build_type=%BUILD_TYPE%
cmake --preset conan-default
cd build/
cmake --build . --config %BUILD_TYPE%
cmake --install .
cd ..