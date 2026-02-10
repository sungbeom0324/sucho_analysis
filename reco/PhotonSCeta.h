// PhotonSCeta.h
#ifndef PHOTON_SCETA_H
#define PHOTON_SCETA_H

#include <cmath>

// "원점(0,0,0) 기준"으로 본 photon eta를 계산 (EB/EE 각각 단순 기하 모델)
// - isScEtaEB: Photon_isScEtaEB
// - eta, phi : Photon_eta, Photon_phi (PV 기준 방향)
// - pvx/y/z  : PV_x, PV_y, PV_z
static inline float ComputeOriginEtaFromPV(bool isScEtaEB,
                                           float eta, float phi,
                                           float pvx, float pvy, float pvz)
{
  float origin_eta = 0.0f;

  // tan(theta/2) = exp(-eta)
  const float pv_tan_theta_over_2 = std::exp(-eta);
  const float pv_tan_theta =
      2.0f * pv_tan_theta_over_2 /
      (1.0f - pv_tan_theta_over_2 * pv_tan_theta_over_2);

  const float photon_unit_x = std::cos(phi);
  const float photon_unit_y = std::sin(phi);

  if (isScEtaEB) {
    // EB: radius ~ 130 cm
    const float pv_ecal_dr = 130.0f - (pvx * photon_unit_x + pvy * photon_unit_y);
    const float pv_ecal_dz = pv_ecal_dr / pv_tan_theta;

    float origin_theta = std::atan(130.0f / (pvz + pv_ecal_dz));
    if (origin_theta < 0) origin_theta += static_cast<float>(M_PI);

    origin_eta = -std::log(std::tan(origin_theta / 2.0f));
  } else {
    // EE: z ~ +/-310 cm
    float pv_ecal_dz = 310.0f - pvz;        // + endcap
    if (eta < 0) pv_ecal_dz = 310.0f + pvz; // - endcap

    const float pv_ecal_dr = pv_ecal_dz * pv_tan_theta;

    float origin_theta = std::atan(((photon_unit_x * pvx + photon_unit_y * pvy) + pv_ecal_dr) / 310.0f);
    if (origin_theta < 0) origin_theta += static_cast<float>(M_PI);

    origin_eta = -std::log(std::tan(origin_theta / 2.0f));
  }

  return origin_eta;
}

#endif

