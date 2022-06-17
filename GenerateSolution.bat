if exist vendor/sfml/lib (
  call vendor\bin\premake\premake5.exe vs2022 
) else (
  echo The repo is missing needed libraries in vendor/. Please make sure you clone the repo recursively as it utilises git submodules.
)

PAUSE