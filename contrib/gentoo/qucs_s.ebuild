# Copyright 1999-2024 Gentoo Authors
# Distributed under the terms of the GNU General Public License v2

EAPI=8

if [[ ${PV} == "9999" ]]; then
	EGIT_REPO_URI="https://github.com/ra3xdh/qucs_s.git"
	inherit git-r3
else
	SRC_URI="https://github.com/ra3xdh/qucs_s/releases/download/${PV}/${P}.tar.gz"
	KEYWORDS="~amd64 ~x86"
fi

inherit cmake-multilib xdg-utils

DESCRIPTION="Quite universal circuit simulator with SPICE"
HOMEPAGE="https://github.com/ra3xdh/qucs_s"

LICENSE="GPL-2"
SLOT="0"
IUSE=""
RESTRICT="mirror"

DEPEND="
		dev-qt/qtsvg:6
		sci-electronics/ngspice
"
RDEPEND="${DEPEND}"
BDEPEND="
		dev-qt/qttools:6
		sys-devel/flex
		sys-devel/bison
		app-text/dos2unix
		dev-util/gperf
"

multilib_src_configure() {
        local mycmakeargs=(
		-DWITH_QT6=ON
		-DCMAKE_INSTALL_PREFIX=/usr                
        )
        cmake_src_configure
}

pkg_postinst() {
		xdg_icon_cache_update
}

pkg_postrm() {
		xdg_icon_cache_update
}

