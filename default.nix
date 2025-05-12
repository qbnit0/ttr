{ pkgs ? import <nixpkgs> {} }:

pkgs.stdenv.mkDerivation {
  name = "ttr";
  src = ./.;
  installPhase = ''
    mkdir -p $out/bin
    cp ttr $out/bin
  '';
}
