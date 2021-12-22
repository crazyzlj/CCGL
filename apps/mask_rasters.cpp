/*!
 * \brief Get subset of input raster(s) according to a mask layer
 *
 * \author Liang-Jun Zhu
 * \date Feb. 2017
 * \changelog
 *     - 1. 2021-11-25 - lj - Rewrite as an stand-alone application inside CCGL
 * 
 * E-mail:  zlj@lreis.ac.cn
 * Copyright (c) 2017-2021. LREIS, IGSNRR, CAS
 *
 */

#if (defined _DEBUG) && (defined _MSC_VER) && (defined VLD)
#include "vld.h"
#endif /* Run Visual Leak Detector during Debug */

#include "data_raster.hpp"

using namespace ccgl;
using namespace data_raster;

/*!
 * \class InputArgs
 * \ingroup module_setting
 * \brief Parse the input arguments of SEIMS.
 */
class InputArgs : Interface {
public:
    /*!
     * \brief Constructor by detail parameters.
     * \param[in] mask_path Path of mask raster
     * \param[in] input_rasters Paths of input rasters
     * \param[in] out_rasters Paths of output rasters
     * \param[in] defvalues Default values for locations that covered by mask but nodata in input raster
     * \param[in] out_datatypes Data types of output rasters
     * \param[in] thread_num thread or processor number, which must be greater or equal than 1
     */
    InputArgs(string& mask_path, vector<string>& input_rasters, vector<string>& out_rasters,
              vector<double>& defvalues, vector<string>& out_datatypes,
              int thread_num = 1);

    /*!
     * \brief Initializer of input arguments.
     * \param[in] argc Number of arguments
     * \param[in] argv \a char* Arguments
     */
    static InputArgs* Init(int argc, const char** argv);

public:
    int thread_num;                    ///< thread number for OpenMP
    string mask_path;                  ///< Path of mask raster
    vector<string> rs_paths;           ///< Paths of input rasters
    vector<string> out_paths;          ///< Paths of output rasters
    vector<double> default_values;     ///< Default values
    vector<string> out_types;          ///< Data types of output rasters
};

void Usage(const string& appname, const string& error_msg = std::string()) {
    if (!error_msg.empty()) {
        cout << "FAILURE: " << error_msg << endl;
    }
    string corename = GetCoreFileName(appname);
    cout << "Simple Usage:\n    " << corename << " <configFile>\n\n";
    cout << "Two complete usage modes are supported:\n";
    cout << "1. " << corename << " -infile <inputRaster> -mask <maskRaster> -defaultvalue <defaultValue>"
            " [-outfile <outputRaster>] [-outtype <outType>] [-thread <threadsNum>]\n\n";
    cout << "2. " << corename << " -configfile <configFile> [-thread <threadsNum>]\n\n";
    cout << "\t<inputRaster> is full path of input raster.\n";
    cout << "\t<maskRaster> is full path of mask raster.\n";
    cout << "\t<defaultValue> is default value for nodata locations that covered by mask.\n";
    cout << "\t<outputRaster> is full path of output raster, if not specified,"
            "<inputRaster>_masked.tif will be generated.\n";
    cout << "\t<outType> is the datatype of <outputRaster>, uint8, int8, uint16,"
            " int16, uint32, int32, float, and double are supported.\n";
    cout << "\t<threadsNum> is the number of thread used by OpenMP, which must be >= 1 (default).\n";
    cout << "\t<configFile> is a plain text file that defines all input parameters, the format is:\n";
    cout << "\t\t<maskRaster>\n";
    cout << "\t\t<rasterCount>\n";
    cout << "\t\t<inputRaster>\t<defaultValue>\t<outputRaster>\t[outType]\n";
    cout << "\t\t...\n\n";
}

InputArgs* InputArgs::Init(const int argc, const char** argv) {
    int thread_num = 1;
    string mask_path;
    vector<string> rs_paths;
    vector<string> out_paths;
    vector<double> default_values;
    vector<string> out_types;

    string config_path;
    bool simple_usage = false;
    if (argc < 2) {
        Usage(argv[0], "To run the program, "
              "use either a single configuration file and/or detail arguments as below.");
        return nullptr;
    }
    if (argc >= 2 && argv[1][0] != '-') {
        config_path = argv[1];
        simple_usage = true;
    }
    int i = 1;
    bool str2num_flag = false;
    while (!simple_usage && argc > i) {
        if (StringMatch(argv[i], "-thread")) {
            i++;
            if (argc > i) {
                thread_num = CVT_INT(IsInt(argv[i], str2num_flag));
                if (!str2num_flag) {
                    Usage(argv[0], "Error: Thread number MUST be an integer!");
                    return nullptr;
                }
                i++;
            }
            else {
                Usage(argv[0]);
                return nullptr;
            }
        }
        else if (StringMatch(argv[i], "-infile")) {
            i++;
            if (argc > i) {
                rs_paths.emplace_back(argv[i]);
                i++;
            }
            else {
                Usage(argv[0]);
                return nullptr;
            }
        }
        else if (StringMatch(argv[i], "-mask")) {
            i++;
            if (argc > i) {
                mask_path = argv[i];
                i++;
            }
            else {
                Usage(argv[0]);
                return nullptr;
            }
        }
        else if (StringMatch(argv[i], "-defaultvalue")) {
            i++;
            if (argc > i) {
                default_values.emplace_back(IsDouble(argv[i], str2num_flag));
                if (!str2num_flag) {
                    Usage(argv[0], "Error: Default value MUST be a number!");
                    return nullptr;
                }
                i++;
            }
            else {
                Usage(argv[0]);
                return nullptr;
            }
        }
        else if (StringMatch(argv[i], "-outfile")) {
            i++;
            if (argc > i) {
                out_paths.emplace_back(argv[i]);
                i++;
            }
            else {
                Usage(argv[0]);
                return nullptr;
            }
        }
        else if (StringMatch(argv[i], "-outtype")) {
            i++;
            if (argc > i) {
                out_types.emplace_back(argv[i]);
                i++;
            }
            else {
                Usage(argv[0]);
                return nullptr;
            }
        }
        else {
            Usage(argv[0], "Error: No matching tag found!");
            return nullptr;
        }
    }
    // Make sure rs_paths, out_paths, default_values, and out_types have same size
    if (!rs_paths.empty()) {
        if (rs_paths.size() != 1) { return nullptr; }
        if (out_paths.empty()) {
            out_paths.emplace_back(AppendCoreFileName(rs_paths[0], "masked"));
        }
        if (default_values.empty()) { default_values.emplace_back(NODATA_VALUE); }
        if (out_types.empty()) { out_types.emplace_back("Unknown"); }
    }
    else { // clear others
        if (!out_paths.empty()) { vector<string>().swap(out_paths); }
        if (!default_values.empty()) { vector<double>().swap(default_values); }
        if (!out_types.empty()) { vector<string>().swap(out_types); }
    }
    // Read configuration file
    if (!config_path.empty()) {
        if (!FileExists(config_path)) {
            Usage(argv[0], "Error: The configuration file MUST be existed!");
            return nullptr;
        }
        vector<string> config_strs;
        if (!LoadPlainTextFile(config_path, config_strs)) {
            Usage(argv[0], "Error: Please follow the format of configuration file!");
            return nullptr;
        }
        size_t line_count = config_strs.size();
        if (line_count < 3) {
            Usage(argv[0], "Error: Configuration file MUST have at least 3 lines!");
            return nullptr;
        }
        string mask_from_cfg = GetAbsolutePath(Trim(config_strs[0]));
        if (!FileExists(mask_from_cfg)) {
            Usage(argv[0], "Error: Mask file defined in conf. file does not existed!");
            return nullptr;
        }
        if (!mask_path.empty() && !StringMatch(GetAbsolutePath(mask_path), mask_from_cfg)) {
            Usage(argv[0], "Error: Mask file defined in arguments and"
                  " configuration file are not the same!");
            return nullptr;
        }
        mask_path = mask_from_cfg;

        size_t inraster_count = CVT_SIZET(IsInt(config_strs[1], str2num_flag));
        if (!str2num_flag) {
            Usage(argv[0], "Error: The 2nd line in config file MUST be an integer!");
            return nullptr;
        }
        size_t icount = rs_paths.size();
        for (size_t si = 2; si < line_count; si++) {
            vector<string> item_strs = SplitString(config_strs[si]);
            size_t item_count = item_strs.size();
            if (item_count < 1) continue;
            if (!FileExists(item_strs[0])) continue;
            rs_paths.emplace_back(item_strs[0]);
            if (item_count >= 2) {
                double tmpdefv = IsDouble(item_strs[1], str2num_flag);
                if (!str2num_flag) default_values.emplace_back(NODATA_VALUE);
                else default_values.emplace_back(tmpdefv);
            } else {
                default_values.emplace_back(NODATA_VALUE);
            }
            if (item_count >= 3) {
                out_paths.emplace_back(item_strs[2]);
            } else {
                out_paths.emplace_back(AppendCoreFileName(item_strs[0], "masked"));
            }
            if (item_count >= 4) {
                out_types.emplace_back(item_strs[3]);
            } else {
                out_types.emplace_back("Unknown");
            }
        }
        icount = out_types.size() - icount;
        if (icount != inraster_count) {
            StatusMessage("Warning: There are " + itoa(CVT_VINT(inraster_count - icount))
                          + " input raster(s) have wrong formats and thus do not be masked!");
        }
    }
    if (mask_path.empty()) {
        Usage(argv[0], "Error: Mask raster MUST be specified!");
        return nullptr;
    }
    return new InputArgs(mask_path, rs_paths, out_paths,
                         default_values, out_types, thread_num);
}

InputArgs::InputArgs(string& mask_path, vector<string>& input_rasters, vector<string>& out_rasters,
                     vector<double>& defvalues, vector<string>& out_datatypes,
                     const int thread_num /* = 1 */) :
    thread_num(thread_num), mask_path(mask_path), rs_paths(input_rasters),
    out_paths(out_rasters), default_values(defvalues), out_types(out_datatypes) {
}

int main(const int argc, const char** argv) {
    InputArgs* input_args = InputArgs::Init(argc, argv);
    if (nullptr == input_args) { return EXIT_FAILURE; }

#ifdef USE_GDAL
    GDALAllRegister();
#endif

    SetOpenMPThread(input_args->thread_num);

    IntRaster* mask_layer = IntRaster::Init(input_args->mask_path);
    if (nullptr == mask_layer) { return EXIT_FAILURE; }

    STRING_MAP opts;
#ifdef HAS_VARIADIC_TEMPLATES
    opts.emplace(HEADER_RSOUT_DATATYPE, "");
#else
    opts.insert(make_pair(HEADER_RSOUT_DATATYPE, ""));
#endif
    for (int i = 0; i < CVT_INT(input_args->rs_paths.size()); i++) {
        cout << input_args->rs_paths[i] << endl;
        opts[HEADER_RSOUT_DATATYPE] = input_args->out_types[i];
        DblIntRaster* rs = DblIntRaster::Init(input_args->rs_paths[i], false,
                                              mask_layer, true,
                                              input_args->default_values[i], opts);
        if (nullptr == rs) {
            cout << "\tError occurred! No masked ouput raster!\n";
            continue;
        }
        rs->OutputToFile(input_args->out_paths[i]);
        delete rs;
    }
    delete mask_layer;
    delete input_args;
    return 0;
}
