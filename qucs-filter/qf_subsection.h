#ifndef QF_SUBSECTION
#define QF_SUBSECTION

#include "qf_math.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace qf {

enum Wiring { SHUNT, SERIES };

enum Content { CAPA, INDUC, PARA_CAPA_INDUC, SERIES_CAPA_INDUC };

struct subsection {
  Wiring wiring;
  Content content;
  qf_float capa_v = 0;
  qf_float indc_v = 0;
  qf_float imp_   = 1;

  void transform_lp(std::vector<subsection>& out_vec, qf_float imp,
                    qf_float fc) {
    qf_float cnrm = 1 / (2 * pi * fc * imp);
    qf_float lnrm = imp / (2 * pi * fc);
    subsection subsec;
    subsec.wiring  = wiring;
    subsec.content = content;
    subsec.capa_v  = capa_v * cnrm;
    subsec.indc_v  = indc_v * lnrm;
    subsec.imp_    = imp;
    out_vec.push_back(subsec);
  }

  void transform_hp(std::vector<subsection>& out_vec, qf_float imp,
                    qf_float fc) {
    subsection subsec;
    qf_float lnrm = imp / (2 * pi * fc);
    qf_float cnrm = 1 / (2 * pi * fc * imp);
    subsec.indc_v = cnrm / capa_v;
    subsec.capa_v = lnrm / indc_v;
    subsec.wiring = wiring;
    subsec.imp_   = imp;
    if (content == CAPA) {
      subsec.content = INDUC;
    } else if (content == INDUC) {
      subsec.content = CAPA;
    } else {
      subsec.content = content;
    }

    out_vec.push_back(subsec);
  }

  void transform_bp(std::vector<subsection>& out_vec, qf_float imp, qf_float fc,
                    qf_float bw) {
    qf_float q    = fc / bw;
    qf_float cnrm = 1 / (2 * pi * fc * imp);
    qf_float lnrm = imp / (2 * pi * fc);
    if (content == CAPA) {
      subsection subsec;
      subsec.wiring  = wiring;
      subsec.content = PARA_CAPA_INDUC;
      subsec.indc_v  = lnrm / (q * capa_v);
      subsec.capa_v  = cnrm * (q * capa_v);
      subsec.imp_    = imp;
      out_vec.push_back(subsec);
    } else if (content == INDUC) {
      subsection subsec;
      subsec.wiring  = wiring;
      subsec.content = SERIES_CAPA_INDUC;
      subsec.capa_v  = lnrm / (q * indc_v);
      subsec.indc_v  = cnrm / (indc_v * q);
      subsec.imp_    = imp;
      out_vec.push_back(subsec);
    } else if (content == PARA_CAPA_INDUC) {
      subsection fst_subsec;
      subsection sec_subsec;
      fst_subsec.wiring  = wiring;
      sec_subsec.wiring  = wiring;
      fst_subsec.imp_    = imp;
      sec_subsec.imp_    = imp;
      fst_subsec.content = PARA_CAPA_INDUC;
      sec_subsec.content = PARA_CAPA_INDUC;
      qf_float iw2       = indc_v * capa_v;
      qf_float b         = sqrt(1 + 4 * q * q * iw2);
      fst_subsec.capa_v  = cnrm * (q * capa_v * 2 * b) / (b + 1);
      fst_subsec.indc_v  = lnrm * (b - 1) / (q * capa_v * 2 * b);
      sec_subsec.capa_v  = cnrm * (q * capa_v * 2 * b) / (b - 1);
      sec_subsec.indc_v  = lnrm * (b + 1) / (q * capa_v * 2 * b);
      out_vec.push_back(fst_subsec);
      out_vec.push_back(sec_subsec);
    }
  }

  void transform_bs(std::vector<subsection>& out_vec, qf_float imp, qf_float fc,
                    qf_float bw) {
    qf_float q    = fc / bw;
    qf_float cnrm = 1 / (2 * pi * fc * imp);
    qf_float lnrm = imp / (2 * pi * fc);
    if (content == CAPA) {
      subsection subsec;
      subsec.wiring  = wiring;
      subsec.content = SERIES_CAPA_INDUC;
      subsec.capa_v  = capa_v * cnrm / q;
      subsec.indc_v  = q * lnrm / capa_v;
      subsec.imp_    = imp;
      out_vec.push_back(subsec);
    } else if (content == INDUC) {
      subsection subsec;
      subsec.wiring  = wiring;
      subsec.content = PARA_CAPA_INDUC;
      subsec.indc_v  = indc_v * lnrm / q;
      subsec.capa_v  = q * cnrm / indc_v;
      subsec.imp_    = imp;
      out_vec.push_back(subsec);
    } else if (content == PARA_CAPA_INDUC) {
      subsection fst_subsec;
      subsection sec_subsec;
      fst_subsec.wiring  = wiring;
      sec_subsec.wiring  = wiring;
      fst_subsec.imp_    = imp;
      sec_subsec.imp_    = imp;
      fst_subsec.content = PARA_CAPA_INDUC;
      sec_subsec.content = PARA_CAPA_INDUC;
      qf_float w2        = 1 / (indc_v * capa_v);
      qf_float b         = sqrt(1 + 4 * q * q * w2);
      fst_subsec.capa_v  = cnrm * (2 * b * q) / (indc_v * (b + 1));
      fst_subsec.indc_v  = lnrm * indc_v * (b - 1) / (2 * b * q);
      sec_subsec.capa_v  = cnrm * (2 * b * q) / (indc_v * (b - 1));
      sec_subsec.indc_v  = lnrm * indc_v * (b + 1) / (2 * b * q);
      out_vec.push_back(fst_subsec);
      out_vec.push_back(sec_subsec);
    }
  }

  void pi_tee_switch() {
    wiring = wiring == Wiring::SHUNT ? Wiring::SERIES : Wiring::SHUNT;
    std::map<Content, Content> content_map = {
        {CAPA, INDUC},
        {INDUC, CAPA},
        {PARA_CAPA_INDUC, SERIES_CAPA_INDUC},
        {SERIES_CAPA_INDUC, PARA_CAPA_INDUC}};
    content = content_map[content];
    std::swap(capa_v, indc_v);
    capa_v /= imp_ * imp_;
    indc_v *= imp_ * imp_;
  }

  void dump() {
    std::string desc = "";
    desc += wiring == Wiring::SHUNT ? "Shunt " : "Series ";
    switch (content) {
    case Content::CAPA:
      desc += "Capa ";
      break;
    case Content::INDUC:
      desc += "Induc ";
      break;
    case Content::SERIES_CAPA_INDUC:
      desc += "Series Capa Induc ";
      break;
    case Content::PARA_CAPA_INDUC:
      desc += "Shunt Capa Induc ";
    default:
      break;
    }
    std::cout << desc << capa_v << " " << indc_v << "\n";
  }
};
} // namespace qf

#endif