#ifndef CCGL_APP_MASK_RASTERIO_H
#define CCGL_APP_MASK_RASTERIO_H

#include "data_raster.hpp"

using namespace ccgl;
using namespace data_raster;

/// Full usage information
void Usage(const string& appname, const string& error_msg = std::string());

/*!
 * \class InputArgs
 * \ingroup module_setting
 * \brief Parse the input arguments of mask_rasterio.
 */
class InputArgs : Interface {
public:
    /*!
     * \brief Constructor by detail parameters.
     * \param[in] mask_path Full path of mask raster
     * \param[in] input_rasters Paths of one or more input raster
     * \param[in] out_rasters Paths of one or more output raster
     * \param[in] defvalues Default values for locations that covered by mask but nodata in input raster
     * \param[in] out_datatypes Data types of one or more output raster
     * \param[in] subset_mode Single (false, default) or multiple (true) subsets of mask
     * \param[in] thread_num thread or processor number, which must be greater or equal than 1
     */
    InputArgs(string& mask_path, vector<string>& input_rasters, vector<string>& out_rasters,
              vector<double>& defvalues, vector<string>& out_datatypes,
              bool subset_mode = true, int thread_num = 1);

    /*!
     * \brief Initializer of input arguments.
     * \param[in] argc Number of arguments
     * \param[in] argv \a char* Arguments
     */
    static InputArgs* Init(int argc, const char** argv);

public:
    int thread_num;                    ///< thread number for OpenMP
    string mask_path;                  ///< Path of mask raster
    bool subset_mode;                  ///< Single (false) or multiple (true) subsets of mask
    vector<string> rs_paths;           ///< Paths of one or more input raster
    vector<string> out_paths;          ///< Paths of one or more output raster
    vector<double> default_values;     ///< Default values
    vector<string> out_types;          ///< Data types of one or more output raster
};

/// Input&Output of masking raster in file formats such as ASC and GeoTiff.
void mask_rasterio(IntRaster* mask_rs, vector<string>& infiles,
                   vector<string>& outtypes, vector<double>& defvalues, vector<string>& outfiles,
                   bool subset_mode = false);

#endif /* CCGL_APP_MASK_RASTERIO_H */
