{
  pkgs ? import <nixpkgs> { },
}:

pkgs.mkShell {
  buildInputs = with pkgs; [
    gcc
    cmake
    gnumake
    pkg-config
    glfw3
    spdlog
    gdb
    libGL
    libX11
    libXrandr
    libXinerama
    libXcursor
    libXi
    libXext
  ];

  nativeBuildInputs = with pkgs; [
    clang-tools
    clang
  ];
}
