#include "UF26Field.h"

#include <cmath>
#include <stdexcept>

// local helper functions and constants
namespace {
  constexpr double kpc = 1;
  constexpr double microgauss = 1;
  // galactocentric radius of the Sun (Gravity Collaboration 2019)
  constexpr double kSunRadius = 8.178 * kpc;
  // arcosh(e): sech(arcosh(e) z/z_d) = 1/e at z = z_d
  const double kArcoshE = std::acosh(std::exp(1.));

  // Gaussian G(x; sigma) of Eq. (16)
  inline
  double
  Gauss(const double x, const double sigma)
  {
    return std::exp(-0.5 * x * x / (sigma * sigma));
  }
}

// initialization of static members
const std::map<UF26Field::ModelType, std::string> UF26Field::fModelNames =
  { {expDisk, "expDisk"},
    {ringDisk, "ringDisk"}
  };

const std::map<std::string, UF26Field::ModelType> UF26Field::fModelTypes =
  { {"expDisk", expDisk},
    {"ringDisk", ringDisk}
  };

UF26Field::UF26Field(const ModelType mt) :
  fModelType(mt)
{
  // common to both models
  fDiskRmax = 18 * kpc;
  fDiskWmax = 2 * kpc;

  switch (fModelType) {
  case expDisk: {
    fDiskB      = 4.4 * microgauss;
    fDiskZ      = 1.0 * kpc;
    fDiskL      = 15 * kpc;
    fDiskRc     = 1 * kpc;
    fHasAnnulus = false;
    break;
  }
  case ringDisk: {
    fDiskB      = 4.1 * microgauss;
    fDiskZ      = 0.9 * kpc;
    fDiskL      = 100 * kpc;
    fDiskRc     = 0;
    fAnnulusB   = 4.1 * microgauss;
    fAnnulusR   = 4.5 * kpc;
    fAnnulusW   = 2.0 * kpc;
    fAnnulusZ   = 1.9 * kpc;
    fHasAnnulus = true;
    break;
  }
  default:
    throw std::runtime_error("UF26Field: unknown model type");
  }

  fSunRadiusSmoothed = std::sqrt(kSunRadius * kSunRadius + fDiskRc * fDiskRc);
}

UF26Field::UF26Field(const std::string& modelName) :
  UF26Field([&modelName]() {
      const auto iter = fModelTypes.find(modelName);
      if (iter == fModelTypes.end())
        throw std::runtime_error("UF26Field: unknown model " + modelName);
      return iter->second;
    }())
{
}

double
UF26Field::operator()(const Vector3& pos)
  const
{
  // rms strengths of independent components add in quadrature
  return std::hypot(GetDiskField(pos), GetAnnularField(pos));
}

double
UF26Field::GetDiskField(const Vector3& pos)
  const
{
  const double r = std::sqrt(pos.x * pos.x + pos.y * pos.y);
  // E_d(r)/E_d(R_sun), Eq. (13)
  const double radial =
    std::exp(-(std::sqrt(r * r + fDiskRc * fDiskRc) - fSunRadiusSmoothed) / fDiskL);
  // outer cutoff S_d(r)
  const double cutoff = 1 / (1 + std::exp((r - fDiskRmax) / fDiskWmax));
  // vertical profile V_d(z), Eq. (14)
  const double vertical = 1 / std::cosh(kArcoshE * pos.z / fDiskZ);
  double b = fDiskB * radial * cutoff * vertical;
  // ringDisk: disk is phased out toward the center following the inner
  // flank of the annulus, G(min(r - r_a, 0); w_a)
  if (fHasAnnulus && r < fAnnulusR)
    b *= Gauss(r - fAnnulusR, fAnnulusW);
  return b;
}

double
UF26Field::GetAnnularField(const Vector3& pos)
  const
{
  if (!fHasAnnulus)
    return 0;
  const double r = std::sqrt(pos.x * pos.x + pos.y * pos.y);
  // vertical Gaussian with 1/e height z_a, i.e. G(z; z_a/sqrt(2))
  const double zz = pos.z / fAnnulusZ;
  return fAnnulusB * Gauss(r - fAnnulusR, fAnnulusW) * std::exp(-zz * zz);
}
