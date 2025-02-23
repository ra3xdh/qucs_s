# Copyright 1999-2025 Gentoo Authors
# Distributed under the terms of the GNU General Public License v2

EAPI=8

inherit cmake-multilib optfeature xdg

DESCRIPTION="Quite universal circuit simulator with SPICE"
HOMEPAGE="https://github.com/ra3xdh/qucs_s"

MY_PN="qucs-s"
MY_P=${MY_PN}-${PV}

if [[ ${PV} == 9999 ]]; then
	EGIT_REPO_URI="https://github.com/ra3xdh/${PN}.git"
	inherit git-r3
else
	SRC_URI="https://github.com/ra3xdh/${PN}/releases/download/${PV}/${MY_P}.tar.gz"
	KEYWORDS="~amd64 ~x86"
fi

LICENSE="GPL-2"
SLOT="0"

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

S="${WORKDIR}/${MY_P}"

DOCS="${S}/AUTHORS ${S}/ChangeLog ${S}/debian/changelog ${S}/CONTRIBUTING.md ${S}/NEWS.md ${S}/NEWS_qucs ${S}/README.md ${S}/README_qucs ${S}/THANKS ${S}/TODO"

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
