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
    libxkbcommon
    wayland
    wayland-protocols
    wayland-scanner
    pkg-config
  ];

  nativeBuildInputs = with pkgs; [
    clang-tools
    clang
  ];

  # GLFW dlopen()s libwayland-client/libxkbcommon/libEGL by soname at
  # runtime instead of linking them, so on NixOS (no /usr/lib) they
  # need to be put on the loader's search path explicitly.
  shellHook = ''
    export LD_LIBRARY_PATH=${
      pkgs.lib.makeLibraryPath [
        pkgs.wayland
        pkgs.libxkbcommon
        pkgs.libGL
      ]
    }''${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}
  '';
}
