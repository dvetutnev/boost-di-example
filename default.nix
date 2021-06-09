{ pkgs ? import <nixpkgs> {} }:

with pkgs; stdenv.mkDerivation {
  name = "fart-checker";
  nativeBuildInputs = [
    cmake
    conan
    ninja
  ];
}
