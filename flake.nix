{
  description = "A very basic flake";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
  };

  outputs = { self, nixpkgs }: let
    withSystem = nixpkgs.lib.genAttrs nixpkgs.lib.platforms.all;
  in {

    packages = withSystem(system: let
      pkgs = nixpkgs.legacyPackages.${system};
    in {
      ttr = (import ./default.nix) { inherit pkgs; };
    default = self.packages.${system}.ttr;
    });
  };
}
