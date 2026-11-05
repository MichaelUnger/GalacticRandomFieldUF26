#ifndef _UF26Field_h_
#define _UF26Field_h_
/**
 @class UF26Field
 @brief UF26 model of the random Galactic magnetic field

 Implements the two random-field models of UF26, expDisk and ringDisk
 (Table 2 of M. Unger and G.R. Farrar, arXiv:2608.21293).

 The models describe the rms strength b of the isotropic random field,
 i.e. <|b|^2> = b^2, where the average is over a volume large compared to
 the coherence length of the field. Each Cartesian component therefore
 has an rms of b/sqrt(3).

 Assumes a galactocentric coordinate system with the Galactic center
 at the origin, the x-axis pointing in the opposite direction of the
 Sun, and the z-axis pointing towards Galactic North. The Sun is at
 a distance of 8.178 kpc from the Galactic center.

 */

#include <map>
#include <string>
#include "Vector3.h"

class UF26Field {
public:
  /// models of Table 2 of the UF26 paper
  enum ModelType {
    expDisk,
    ringDisk
  };

  static const std::string& GetModelName(const ModelType m)
  { return fModelNames.at(m); }
  static const std::map<ModelType, std::string>& GetModelNames()
  { return fModelNames; }
  const std::string& GetModelName() const
  { return fModelNames.at(fModelType); }

public:
  /**
     @brief constructor
     @param mt model type (expDisk or ringDisk)
  */
  UF26Field(const ModelType mt);
  UF26Field(const std::string& modelName);

  /// no default constructor
  UF26Field() = delete;

  /**
     @brief rms strength of the random field at a given position
     @param posInKpc position with components given in kpc
     @return rms field strength b in microgauss
  */
  double operator()(const Vector3& posInKpc) const;

  /// rms strength of the disk component (Eq. 12) in microgauss
  double GetDiskField(const Vector3& posInKpc) const;
  /// rms strength of the annular component (Eq. 15) in microgauss
  double GetAnnularField(const Vector3& posInKpc) const;

private:
  /// model type given in constructor
  const ModelType fModelType;

  // disk, Eqs. (12)-(14) [kpc, microgauss]
  double fDiskB      = 0;  ///< b_d at the solar circle
  double fDiskZ      = 0;  ///< 1/e height z_d
  double fDiskL      = 0;  ///< radial scale length l_r
  double fDiskRc     = 0;  ///< cusp smoothing radius r_c
  double fDiskRmax   = 0;  ///< outer cutoff radius R_max
  double fDiskWmax   = 0;  ///< width of outer cutoff w_max
  // annulus, Eq. (15) [kpc, microgauss]
  double fAnnulusB   = 0;  ///< peak amplitude b_a,0
  double fAnnulusR   = 0;  ///< peak radius r_a
  double fAnnulusW   = 0;  ///< radial Gaussian width w_a
  double fAnnulusZ   = 0;  ///< 1/e height z_a
  bool fHasAnnulus = false;

  // pre-calculated: sqrt(R_sun^2 + r_c^2)
  double fSunRadiusSmoothed = 0;

  static const std::map<ModelType, std::string> fModelNames;
  static const std::map<std::string, ModelType> fModelTypes;
};
#endif
