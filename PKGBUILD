pkgname=libexec-git
pkgdesc="Execute Programs"
pkgver=r4.3a94eed
pkgrel=1

source=("git+https://github.com/kkernick/libexec.git")
b2sums=("SKIP")
depends=()
makedepends=(git lld clang doxygen)
optdepends=()
arch=("any")
provides=("libexec")
conflicts=("libexec")

pkgver() {
  cd $srcdir/libexec
  printf "r%s.%s" "$(git rev-list --count HEAD)" "$(git rev-parse --short=7 HEAD)"
}

build() {
  cd $srcdir/libexec
  make
  make docs
}

package() {
  cd $srcdir/libexec
  install -Dm755 "libexec.so" "$pkgdir/usr/lib/libexec.so"
  install -Dm644 "src/exec.hpp" "$pkgdir/usr/include/exec.hpp"
  install -Dm644 "LICENSE" "$pkgdir/usr/share/licenses/libexec/LICENSE"

  for page in $srcdir/libexec/docs/man/man3/*; do
    gzip $page
    install -Dm644 "$page.gz" "$pkgdir/usr/share/man/man3/libexec.$(basename $page).gz"
  done
}
