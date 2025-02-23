# Copyright 1999-2025 Gentoo Authors
# Distributed under the terms of the GNU General Public License v2

EAPI=8

if [[ ${PV} == "9999" ]]; then
	EGIT_REPO_URI="https://github.com/ra3xdh/qucs_s.git"
	inherit git-r3
else
	SRC_URI="https://github.com/ra3xdh/qucs_s/releases/download/${PV}/${P}.tar.gz"
	KEYWORDS="~amd64 ~x86"
fi

inherit cmake-multilib optfeature xdg

DESCRIPTION="Quite universal circuit simulator with SPICE"
HOMEPAGE="https://github.com/ra3xdh/qucs_s"

LICENSE="GPL-2"
SLOT="0"
IUSE=""
RESTRICT=""

DEPEND="
	dev-qt/qtbase:6[gui,widgets]
	dev-qt/qtsvg:6
"
RDEPEND="${DEPEND}"
BDEPEND="
	dev-qt/qttools:6[linguist]
	sys-devel/flex
	sys-devel/bison
	dev-util/gperf
	app-text/dos2unix
"

multilib_src_configure() {
        local mycmakeargs=(
		-DCMAKE_INSTALL_PREFIX=/usr                
        )
        cmake_src_configure
}

pkg_preinst() {
	xdg_pkg_preinst
}

pkg_postinst() {
	optfeature "Result postprocessing in Octave" sci-mathematics/octave

	optfeature_header "Install optonal simulator backends:"
	optfeature "Ngspice" sci-electronics/ngspice

	xdg_pkg_postinst
}

pkg_postrm() {
	xdg_pkg_postrm
}
