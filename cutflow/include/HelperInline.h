#ifndef HELPER_INLINE
#define HELPER_INLINE 

static inline float ConvertMVA(float mva_mini) {
  return 2.0f / (1.0f + std::exp(-2.0f * mva_mini)) - 1.0f;
}

static inline bool HzzId_WP2022(float pt, float etasc, float hzzmvaid) {
  const float aeta = std::fabs(etasc);

  if (pt < 10.0f) {
    if (aeta < 0.8f)         return (hzzmvaid > ConvertMVA(1.6339f));
    else if (aeta < 1.479f)  return (hzzmvaid > ConvertMVA(1.5499f));
    else                     return (hzzmvaid > ConvertMVA(2.0629f));
  } else {
    if (aeta < 0.8f)         return (hzzmvaid > ConvertMVA(0.3685f));
    else if (aeta < 1.479f)  return (hzzmvaid > ConvertMVA(0.2662f));
    else                     return (hzzmvaid > ConvertMVA(-0.5444f));
  }
}

#endif
