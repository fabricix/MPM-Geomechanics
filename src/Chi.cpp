#include "Chi.h"
#include <algorithm>

namespace Chi 
{
    /// @brief Configuration for chi model
    Chi::Parameters chiParameters;

    /// \brief Get chi model configuration
	/// \param[in] chi Chi model configuration
	void setParameters(Chi::Parameters chi) {
        chiParameters = chi;
    }

	/// \brief Get chi model type
	Chi::Model getChiModelType(){
        return chiParameters.model;
    }

    /// \brief Compute chi from saturation ratio Sr
    double getChiFromSr(double Sr) {

        Sr = std::clamp(Sr, 0.0, 1.0);
        
        switch (chiParameters.model) {
        
            case Chi::Model::CONSTANT:
                return std::clamp(chiParameters.chi_const, 0.0, 1.0);
        
            case Chi::Model::SE: 
            {
                const double denom = std::max(1e-12, 1.0 - chiParameters.sr_res);
                return std::clamp((Sr - chiParameters.sr_res)/denom, 0.0, 1.0);
            }
            
            case Chi::Model::SR:
            default:
                return Sr;
        }
    }
}