{
  pkgs ? import <nixpkgs> {},
  ...
}: pkgs.mkShell {
  buildInputs = [
    pkgs.gcc
    pkgs.gdb
    pkgs.openssl
    pkgs.pkg-config
  ];
}
