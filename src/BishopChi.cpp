#include "BishopChi.h"
#include <algorithm>

namespace BishopChi 
{
    /// @brief Configuration for chi model
    BishopChi::Parameters bishopChiParameters;

    /// \brief Get chi model configuration
	/// \param[in] chi Chi model configuration
	void setParameters(BishopChi::Parameters chi) {
        bishopChiParameters = chi;
    }

	/// \brief Get chi model type
	BishopChi::Model getChiModelType(){
        return bishopChiParameters.model;
    }

    /// \brief Compute chi from saturation ratio Sr
    double getChiFromSr(double Sr) {

        Sr = std::clamp(Sr, 0.0, 1.0);
        
        switch (bishopChiParameters.model) {
        
            case BishopChi::Model::CONSTANT:
                return std::clamp(bishopChiParameters.chi_const, 0.0, 1.0);
        
            case BishopChi::Model::SE: 
            {
                const double denom = std::max(1e-12, 1.0 - bishopChiParameters.sr_res);
                return std::clamp((Sr - bishopChiParameters.sr_res)/denom, 0.0, 1.0);
            }
            
            case BishopChi::Model::SR:
            default:
                return Sr;
        }
    }
}