{
  pkgs ? import <nixpkgs> { },
}:

pkgs.mkShell {
  buildInputs = with pkgs; [
    gcc
    cmake
    gnumake
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
