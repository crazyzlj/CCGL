/*!
 * \brief Test description:
 *
 *        TEST CASE NAME (or TEST SUITE):
 *            clsRasterDataTestMaskWithin: Read raster with a mask that within raster's extent.
 *
 *            Once a raster used as a mask, its position data must be existed or recalculated.
 *
 *        P.S.1. Copy constructor is also tested here.
 *        P.S.2. MongoDB I/O is also tested if mongo-c-driver configured.
 *
 *        Since we mainly support ASC and GDAL(e.g., TIFF),
 *        value-parameterized tests of Google Test will be used.
 * \cite https://github.com/google/googletest/blob/master/googletest/samples/sample7_unittest.cc
 * \version 1.3
 * \authors Liangjun Zhu, zlj(at)lreis.ac.cn; crazyzlj(at)gmail.com
 * \remarks 2017-12-02 - lj - Original version.
 *          2018-05-03 - lj - Integrated into CCGL.
 *          2019-11-06 - lj - Allow user specified MongoDB host and port.
 *          2021-07-20 - lj - Update after changes of GetValue and GetValueByIndex.
 *          2021-11-18 - lj - Rewrite unittest cases, avoid redundancy. 
 *
 */
#include "gtest/gtest.h"
#include "../../src/data_raster.hpp"
#include "../../src/utils_filesystem.h"
#include "../../src/utils_time.h"
#include "../../src/utils_string.h"
#include "../../src/utils_array.h"
#ifdef USE_MONGODB
#include "../../src/db_mongoc.h"
#endif
#include "../test_global.h"

using namespace ccgl::data_raster;
using namespace ccgl::utils_filesystem;
using namespace ccgl::utils_time;
using namespace ccgl::utils_string;
using namespace ccgl::utils_array;
#ifdef USE_MONGODB
using namespace ccgl::db_mongoc;
#endif

extern GlobalEnvironment* GlobalEnv;

namespace {
using ::testing::TestWithParam;
using ::testing::Values;

string Apppath = GetAppPath();
string Corename = "tinydemo_raster_r4c3";
string MaskcorenameS = "tinydemo_mask_r2c2"; // within extent of raster data

string AscFile = Apppath + "./data/raster/" + Corename + ".asc";
string TifFile = Apppath + "./data/raster/" + Corename + ".tif";
string MaskAscFileS = Apppath + "./data/raster/" + MaskcorenameS + ".asc";
string MaskTifFileS = Apppath + "./data/raster/" + MaskcorenameS + ".tif";

struct InputRasterFiles {
public:
    InputRasterFiles(const string& rsf, const string& maskf) {
        raster_name = rsf.c_str();
        mask_name = maskf.c_str();
    }
    const char* raster_name;
    const char* mask_name;
};

//Inside the test body, fixture constructor, SetUp(), and TearDown() you
//can refer to the test parameter by GetParam().  In this case, the test
//parameter is a factory function which we call in fixture's SetUp() to
//create and store an instance of clsRasterData<float>.
class clsRasterDataTestMaskWithin: public TestWithParam<InputRasterFiles *> {
public:
    clsRasterDataTestMaskWithin() : rs_(nullptr), maskrs_(nullptr) {
    }

    virtual ~clsRasterDataTestMaskWithin() {
        delete rs_;
        delete maskrs_;
        rs_ = nullptr;
        maskrs_ = nullptr;
    }

    void SetUp() OVERRIDE {
        // Read mask data without calculate valid positions
        maskrs_ = IntRaster::Init(GetParam()->mask_name, false);
        ASSERT_NE(nullptr, maskrs_);
        EXPECT_FALSE(maskrs_->PositionsCalculated());
        // Read raster data with mask, do not calculate positions, and do not use mask's extent
        rs_ = FltIntRaster::Init(GetParam()->raster_name, false, maskrs_, false);
        ASSERT_NE(nullptr, rs_);
        EXPECT_TRUE(maskrs_->PositionsCalculated());
    }

    void TearDown() OVERRIDE {
    }

protected:
    FltIntRaster* rs_;
    IntRaster* maskrs_;
};

// calc_pos = False, use_mask_ext = False
TEST_P(clsRasterDataTestMaskWithin, NoPosNoMaskExt) {
    /// Mask data
    EXPECT_TRUE(maskrs_->PositionsCalculated());
    EXPECT_EQ(2, maskrs_->GetValidNumber());
    EXPECT_EQ(2, maskrs_->GetCellNumber());
    EXPECT_EQ(2, maskrs_->GetDataLength());

    EXPECT_EQ(-9999., maskrs_->GetValueByIndex(-1));
    EXPECT_EQ(1, maskrs_->GetValueByIndex(0));
    EXPECT_EQ(1, maskrs_->GetValueByIndex(1));
    EXPECT_EQ(-9999, maskrs_->GetValueByIndex(2));
    EXPECT_EQ(-9999, maskrs_->GetValueByIndex(3));

    /// Test members after constructing.
    EXPECT_EQ(4, rs_->GetDataLength()); // m_nCells, which will be nRows * nCols
    EXPECT_EQ(4, rs_->GetCellNumber()); // m_nCells

    EXPECT_FLOAT_EQ(-9999.f, rs_->GetNoDataValue());  // m_noDataValue
    EXPECT_FLOAT_EQ(-9999.f, rs_->GetDefaultValue()); // m_defaultValue

    // m_filePathName depends on the path of build, so no need to test.
    EXPECT_EQ(Corename, rs_->GetCoreName()); // m_coreFileName

    EXPECT_TRUE(rs_->Initialized());           // m_initialized
    EXPECT_FALSE(rs_->Is2DRaster());           // m_is2DRaster
    EXPECT_FALSE(rs_->PositionsCalculated());  // m_calcPositions
    EXPECT_FALSE(rs_->PositionsAllocated());   // m_storePositions
    EXPECT_FALSE(rs_->MaskExtented());         // m_useMaskExtent
    EXPECT_FALSE(rs_->StatisticsCalculated()); // m_statisticsCalculated

    ASSERT_TRUE(rs_->ValidateRasterData());

    EXPECT_NE(nullptr, rs_->GetRasterDataPointer());         // m_rasterData
    EXPECT_EQ(nullptr, rs_->Get2DRasterDataPointer());       // m_raster2DData
    EXPECT_EQ(nullptr, rs_->GetRasterPositionDataPointer()); // m_rasterPositionData

    /** Get metadata, m_headers **/
    map<string, double> header_info = rs_->GetRasterHeader();
    EXPECT_EQ(CVT_INT(header_info.at(HEADER_RS_LAYERS)), rs_->GetLayers());
    EXPECT_EQ(CVT_INT(header_info.at(HEADER_RS_CELLSNUM)), rs_->GetCellNumber());

    EXPECT_EQ(2, rs_->GetRows());
    EXPECT_EQ(2, rs_->GetCols());
    EXPECT_DOUBLE_EQ(3., rs_->GetXllCenter());
    EXPECT_DOUBLE_EQ(3., rs_->GetYllCenter());
    EXPECT_DOUBLE_EQ(2., rs_->GetCellWidth());
    EXPECT_EQ(1, rs_->GetLayers());
    EXPECT_EQ("", rs_->GetSrsString());

    /** Calc and get basic statistics, m_statsMap **/
    EXPECT_EQ(2, rs_->GetValidNumber());
    EXPECT_DOUBLE_EQ(4., rs_->GetMinimum());
    EXPECT_DOUBLE_EQ(5., rs_->GetMaximum());
    EXPECT_DOUBLE_EQ(4.5, rs_->GetAverage());
    EXPECT_DOUBLE_EQ(0.5, rs_->GetStd());
    EXPECT_DOUBLE_EQ(1., rs_->GetRange());
    EXPECT_TRUE(rs_->StatisticsCalculated());

    EXPECT_NE(nullptr, rs_->GetMask()); // m_mask

    /** Test getting raster data **/
    int ncells = 0;
    float* rs_data = nullptr;
    EXPECT_TRUE(rs_->GetRasterData(&ncells, &rs_data)); // m_rasterData
    EXPECT_EQ(4, ncells);
    EXPECT_NE(nullptr, rs_data);
    EXPECT_FLOAT_EQ(-9999.f, rs_data[0]);
    EXPECT_FLOAT_EQ(4.f, rs_data[1]);
    EXPECT_FLOAT_EQ(5.f, rs_data[2]);
    EXPECT_FLOAT_EQ(-9999.f, rs_data[3]);

    float** rs_2ddata = nullptr;
    int nlyrs = -1;
    EXPECT_FALSE(rs_->Get2DRasterData(&ncells, &nlyrs, &rs_2ddata)); // m_raster2DData
    EXPECT_EQ(-1, ncells);
    EXPECT_EQ(-1, nlyrs);
    EXPECT_EQ(nullptr, rs_2ddata);

    /** Get raster cell value by various way **/
    /// Get cell value by index of raster_data_
    EXPECT_FLOAT_EQ(-9999.f, rs_->GetValueByIndex(-1));
    EXPECT_FLOAT_EQ(-9999.f, rs_->GetValueByIndex(0));
    EXPECT_FLOAT_EQ(4.f, rs_->GetValueByIndex(1));
    EXPECT_FLOAT_EQ(5.f, rs_->GetValueByIndex(2));
    EXPECT_FLOAT_EQ(-9999.f, rs_->GetValueByIndex(3));
    EXPECT_FLOAT_EQ(-9999.f, rs_->GetValueByIndex(4));

    /// Get cell value by index of raster_data_/raster_2d_, compatible way
    int tmp_lyr = rs_->GetLayers();
    float* tmp_values = nullptr;
    Initialize1DArray(tmp_lyr, tmp_values, -9999.f);
    rs_->GetValueByIndex(-1, tmp_values); // index exceed, tmp_values will be released
    EXPECT_EQ(nullptr, tmp_values);
    rs_->GetValueByIndex(0, tmp_values); // nullptr input, tmp_values will be initialized
    EXPECT_NE(nullptr, tmp_values);
    EXPECT_FLOAT_EQ(-9999.f, tmp_values[0]);
    rs_->GetValueByIndex(2, tmp_values);
    EXPECT_NE(nullptr, tmp_values);
    EXPECT_FLOAT_EQ(5.f, tmp_values[0]);

    /// Get cell value by (row, col)
    EXPECT_FLOAT_EQ(-9999.f, rs_->GetValue(-1, 0));
    EXPECT_FLOAT_EQ(-9999.f, rs_->GetValue(0, 0));
    EXPECT_FLOAT_EQ(4.f, rs_->GetValue(0, 1));
    EXPECT_FLOAT_EQ(5.f, rs_->GetValue(1, 0));
    EXPECT_FLOAT_EQ(-9999.f, rs_->GetValue(1, 1));

    /// Get cell values by (row, col, *&array)
    rs_->GetValue(-1, 0, tmp_values);
    EXPECT_EQ(nullptr, tmp_values);
    rs_->GetValue(0, -1, tmp_values);
    EXPECT_EQ(nullptr, tmp_values);
    rs_->GetValue(0, 0, tmp_values);
    EXPECT_NE(nullptr, tmp_values);
    EXPECT_FLOAT_EQ(-9999.f, tmp_values[0]);
    rs_->GetValue(0, 1, tmp_values);
    EXPECT_NE(nullptr, tmp_values);
    EXPECT_FLOAT_EQ(4.f, tmp_values[0]);
    rs_->GetValue(1, 0, tmp_values);
    EXPECT_NE(nullptr, tmp_values);
    EXPECT_FLOAT_EQ(5.f, tmp_values[0]);

    Release1DArray(tmp_values);
    EXPECT_EQ(nullptr, tmp_values);

    // Get position index, if cal_poc is False, no chance to return -1
    EXPECT_EQ(-2, rs_->GetPosition(-1., -2.));
    EXPECT_EQ(-2, rs_->GetPosition(-1., 9.));
    EXPECT_EQ(-2, rs_->GetPosition(1., 7.));
    EXPECT_EQ(-2, rs_->GetPosition(3.99, 7.05));
    EXPECT_EQ(-2, rs_->GetPosition(5.01, 7.00));
    EXPECT_EQ(-2, rs_->GetPosition(0.01, 5.5));
    EXPECT_EQ(0, rs_->GetPosition(2.01, 5.1));
    EXPECT_EQ(1, rs_->GetPosition(4.5, 5.5));
    EXPECT_EQ(-2, rs_->GetPosition(1., 3.));
    EXPECT_EQ(2, rs_->GetPosition(3., 3.));
    EXPECT_EQ(3, rs_->GetPosition(5., 3.));
    EXPECT_EQ(-2, rs_->GetPosition(1., 1.));
    EXPECT_EQ(-2, rs_->GetPosition(3., 1.));
    EXPECT_EQ(-2, rs_->GetPosition(5., 1.));

    /** Set value **/
    // Set core file name
    string newcorename = Corename + "_1D-mask-within-nopos-nomaskext";
    rs_->SetCoreName(newcorename);
    EXPECT_EQ(newcorename, rs_->GetCoreName());

    /** Output to new file **/
    string oldfullname = rs_->GetFilePath();
    string newfullname = GetPathFromFullName(oldfullname) + "result" + SEP +
            newcorename + "." + GetSuffix(oldfullname);
    EXPECT_TRUE(rs_->OutputToFile(newfullname));
    EXPECT_TRUE(FileExists(newfullname));

    /** Copy constructor **/
    FltIntRaster* copyrs = new FltIntRaster(rs_);

    FltIntRaster copyrs2(rs_);

    FltIntRaster copyrs3;
    copyrs3.Copy(rs_);

    // Selected tests
    EXPECT_EQ(4, copyrs->GetCellNumber()); // m_nCells
    EXPECT_EQ(1, copyrs->GetLayers());
    EXPECT_EQ(2, copyrs->GetValidNumber());

    EXPECT_EQ(copyrs->GetMask(), copyrs2.GetMask());
    EXPECT_EQ(copyrs->GetMask(), copyrs3.GetMask());

#ifdef USE_MONGODB
    /** MongoDB I/O test **/
    MongoClient* conn = MongoClient::Init(GlobalEnv->mongoHost.c_str(), GlobalEnv->mongoPort);
    if (nullptr != conn) {
        string gfsfilename = newcorename + "_" + GetSuffix(oldfullname);
        MongoGridFs* gfs = new MongoGridFs(conn->GetGridFs("test", "spatial"));
        gfs->RemoveFile(gfsfilename);
        copyrs->OutputToMongoDB(gfs, gfsfilename);
        double stime = TimeCounting();
        FltIntRaster* mongors = FltIntRaster::Init(gfs, gfsfilename.c_str(),
                                                   false, maskrs_, false);
        cout << "Reading parameter finished, TIMESPAN " << ValueToString(TimeCounting() - stime) << " sec." << endl;
        // test mongors data
        EXPECT_EQ(4, mongors->GetCellNumber()); // m_nCells
        EXPECT_EQ(1, mongors->GetLayers());
        EXPECT_EQ(2, mongors->GetValidNumber());
        EXPECT_DOUBLE_EQ(4.5, mongors->GetAverage());
        // output to asc/tif file for comparison

        string newfullname4mongo = GetPathFromFullName(oldfullname) + "result" + SEP +
                newcorename + "_mongo." + GetSuffix(oldfullname);
        EXPECT_TRUE(rs_->OutputToFile(newfullname4mongo));
        EXPECT_TRUE(FileExists(newfullname4mongo));

        delete mongors;
        delete gfs;
    }
    //conn->Destroy(); // the MongoClient MUST not be destroyed or deleted!
    //delete conn;
#endif
    delete copyrs;
    // No need to release copyrs2 and copyrs3 by developers.
}

// calc_pos = False, use_mask_ext = True
TEST_P(clsRasterDataTestMaskWithin, NoPosUseMaskExt) {
    /// Mask data
    EXPECT_TRUE(maskrs_->PositionsCalculated());
    EXPECT_EQ(2, maskrs_->GetValidNumber());
    EXPECT_EQ(2, maskrs_->GetCellNumber());
    EXPECT_EQ(2, maskrs_->GetDataLength());

    EXPECT_EQ(-9999., maskrs_->GetValueByIndex(-1));
    EXPECT_EQ(1, maskrs_->GetValueByIndex(0));
    EXPECT_EQ(1, maskrs_->GetValueByIndex(1));
    EXPECT_EQ(-9999, maskrs_->GetValueByIndex(2));
    EXPECT_EQ(-9999, maskrs_->GetValueByIndex(3));

    /// Set use_mask_ext
    EXPECT_FALSE(rs_->MaskExtented());
    rs_->SetUseMaskExt();
    EXPECT_TRUE(rs_->MaskExtented());

    /// Test members after constructing.
    EXPECT_EQ(4, rs_->GetDataLength()); // m_nCells, which will be nRows * nCols
    EXPECT_EQ(4, rs_->GetCellNumber()); // m_nCells

    EXPECT_FLOAT_EQ(-9999.f, rs_->GetNoDataValue());  // m_noDataValue
    EXPECT_FLOAT_EQ(-9999.f, rs_->GetDefaultValue()); // m_defaultValue

    // m_filePathName depends on the path of build, so no need to test.
    EXPECT_EQ(Corename, rs_->GetCoreName()); // m_coreFileName

    EXPECT_TRUE(rs_->Initialized());           // m_initialized
    EXPECT_FALSE(rs_->Is2DRaster());           // m_is2DRaster
    EXPECT_FALSE(rs_->PositionsCalculated());  // m_calcPositions
    EXPECT_FALSE(rs_->PositionsAllocated());   // m_storePositions
    EXPECT_TRUE(rs_->MaskExtented());          // m_useMaskExtent
    EXPECT_FALSE(rs_->StatisticsCalculated()); // m_statisticsCalculated

    ASSERT_TRUE(rs_->ValidateRasterData());

    EXPECT_NE(nullptr, rs_->GetRasterDataPointer());         // m_rasterData
    EXPECT_EQ(nullptr, rs_->Get2DRasterDataPointer());       // m_raster2DData
    EXPECT_EQ(nullptr, rs_->GetRasterPositionDataPointer()); // m_rasterPositionData

    /** Get metadata, m_headers **/
    map<string, double> header_info = rs_->GetRasterHeader();
    EXPECT_EQ(CVT_INT(header_info.at(HEADER_RS_LAYERS)), rs_->GetLayers());
    EXPECT_EQ(CVT_INT(header_info.at(HEADER_RS_CELLSNUM)), rs_->GetCellNumber());

    EXPECT_EQ(2, rs_->GetRows());
    EXPECT_EQ(2, rs_->GetCols());
    EXPECT_DOUBLE_EQ(3., rs_->GetXllCenter());
    EXPECT_DOUBLE_EQ(3., rs_->GetYllCenter());
    EXPECT_DOUBLE_EQ(2., rs_->GetCellWidth());
    EXPECT_EQ(1, rs_->GetLayers());
    EXPECT_EQ("", rs_->GetSrsString());

    /** Calc and get basic statistics, m_statsMap **/
    EXPECT_EQ(2, rs_->GetValidNumber());
    EXPECT_DOUBLE_EQ(4., rs_->GetMinimum());
    EXPECT_DOUBLE_EQ(5., rs_->GetMaximum());
    EXPECT_DOUBLE_EQ(4.5, rs_->GetAverage());
    EXPECT_DOUBLE_EQ(0.5, rs_->GetStd());
    EXPECT_DOUBLE_EQ(1., rs_->GetRange());
    EXPECT_TRUE(rs_->StatisticsCalculated());

    EXPECT_NE(nullptr, rs_->GetMask()); // m_mask

    /** Test getting raster data **/
    int ncells = 0;
    float* rs_data = nullptr;
    EXPECT_TRUE(rs_->GetRasterData(&ncells, &rs_data)); // m_rasterData
    EXPECT_EQ(4, ncells);
    EXPECT_NE(nullptr, rs_data);
    EXPECT_FLOAT_EQ(-9999.f, rs_data[0]);
    EXPECT_FLOAT_EQ(4.f, rs_data[1]);
    EXPECT_FLOAT_EQ(5.f, rs_data[2]);
    EXPECT_FLOAT_EQ(-9999.f, rs_data[3]);

    float** rs_2ddata = nullptr;
    int nlyrs = -1;
    EXPECT_FALSE(rs_->Get2DRasterData(&ncells, &nlyrs, &rs_2ddata)); // m_raster2DData
    EXPECT_EQ(-1, ncells);
    EXPECT_EQ(-1, nlyrs);
    EXPECT_EQ(nullptr, rs_2ddata);

    /** Get raster cell value by various way **/
    /// Get cell value by index of raster_data_
    EXPECT_FLOAT_EQ(-9999.f, rs_->GetValueByIndex(-1));
    EXPECT_FLOAT_EQ(-9999.f, rs_->GetValueByIndex(0));
    EXPECT_FLOAT_EQ(4.f, rs_->GetValueByIndex(1));
    EXPECT_FLOAT_EQ(5.f, rs_->GetValueByIndex(2));
    EXPECT_FLOAT_EQ(-9999.f, rs_->GetValueByIndex(3));
    EXPECT_FLOAT_EQ(-9999.f, rs_->GetValueByIndex(4));

    /// Get cell value by index of raster_data_/raster_2d_, compatible way
    int tmp_lyr = rs_->GetLayers();
    float* tmp_values = nullptr;
    Initialize1DArray(tmp_lyr, tmp_values, -9999.f);
    rs_->GetValueByIndex(-1, tmp_values); // index exceed, tmp_values will be released
    EXPECT_EQ(nullptr, tmp_values);
    rs_->GetValueByIndex(0, tmp_values); // nullptr input, tmp_values will be initialized
    EXPECT_NE(nullptr, tmp_values);
    EXPECT_FLOAT_EQ(-9999.f, tmp_values[0]);
    rs_->GetValueByIndex(2, tmp_values);
    EXPECT_NE(nullptr, tmp_values);
    EXPECT_FLOAT_EQ(5.f, tmp_values[0]);

    /// Get cell value by (row, col)
    EXPECT_FLOAT_EQ(-9999.f, rs_->GetValue(-1, 0));
    EXPECT_FLOAT_EQ(-9999.f, rs_->GetValue(0, 0));
    EXPECT_FLOAT_EQ(4.f, rs_->GetValue(0, 1));
    EXPECT_FLOAT_EQ(5.f, rs_->GetValue(1, 0));
    EXPECT_FLOAT_EQ(-9999.f, rs_->GetValue(1, 1));

    /// Get cell values by (row, col, *&array)
    rs_->GetValue(-1, 0, tmp_values);
    EXPECT_EQ(nullptr, tmp_values);
    rs_->GetValue(0, -1, tmp_values);
    EXPECT_EQ(nullptr, tmp_values);
    rs_->GetValue(0, 0, tmp_values);
    EXPECT_NE(nullptr, tmp_values);
    EXPECT_FLOAT_EQ(-9999.f, tmp_values[0]);
    rs_->GetValue(0, 1, tmp_values);
    EXPECT_NE(nullptr, tmp_values);
    EXPECT_FLOAT_EQ(4.f, tmp_values[0]);
    rs_->GetValue(1, 0, tmp_values);
    EXPECT_NE(nullptr, tmp_values);
    EXPECT_FLOAT_EQ(5.f, tmp_values[0]);

    Release1DArray(tmp_values);
    EXPECT_EQ(nullptr, tmp_values);

    // Get position index, if cal_poc is False, no chance to return -1
    EXPECT_EQ(-2, rs_->GetPosition(-1., -2.));
    EXPECT_EQ(-2, rs_->GetPosition(-1., 9.));
    EXPECT_EQ(-2, rs_->GetPosition(1., 7.));
    EXPECT_EQ(-2, rs_->GetPosition(3.99, 7.05));
    EXPECT_EQ(-2, rs_->GetPosition(5.01, 7.00));
    EXPECT_EQ(-2, rs_->GetPosition(0.01, 5.5));
    EXPECT_EQ(0, rs_->GetPosition(2.01, 5.1));
    EXPECT_EQ(1, rs_->GetPosition(4.5, 5.5));
    EXPECT_EQ(-2, rs_->GetPosition(1., 3.));
    EXPECT_EQ(2, rs_->GetPosition(3., 3.));
    EXPECT_EQ(3, rs_->GetPosition(5., 3.));
    EXPECT_EQ(-2, rs_->GetPosition(1., 1.));
    EXPECT_EQ(-2, rs_->GetPosition(3., 1.));
    EXPECT_EQ(-2, rs_->GetPosition(5., 1.));

    /** Set value **/
    // Set core file name
    string newcorename = Corename + "_1D-mask-within";
    rs_->SetCoreName(newcorename);
    EXPECT_EQ(newcorename, rs_->GetCoreName());

    /** Output to new file **/
    string oldfullname = rs_->GetFilePath();
    string newfullname = GetPathFromFullName(oldfullname) + "result" + SEP +
            newcorename + "." + GetSuffix(oldfullname);
    EXPECT_TRUE(rs_->OutputToFile(newfullname));
    EXPECT_TRUE(FileExists(newfullname));

    /** Copy constructor **/
    FltIntRaster* copyrs = new FltIntRaster(rs_);

    FltIntRaster copyrs2(rs_);

    FltIntRaster copyrs3;
    copyrs3.Copy(rs_);

    // Selected tests
    EXPECT_EQ(4, copyrs->GetCellNumber()); // m_nCells
    EXPECT_EQ(1, copyrs->GetLayers());
    EXPECT_EQ(2, copyrs->GetValidNumber());

    EXPECT_EQ(copyrs->GetMask(), copyrs2.GetMask());
    EXPECT_EQ(copyrs->GetMask(), copyrs3.GetMask());

#ifdef USE_MONGODB
    /** MongoDB I/O test **/
    MongoClient* conn = MongoClient::Init(GlobalEnv->mongoHost.c_str(), GlobalEnv->mongoPort);
    if (nullptr != conn) {
        string gfsfilename = newcorename + "_" + GetSuffix(oldfullname);
        MongoGridFs* gfs = new MongoGridFs(conn->GetGridFs("test", "spatial"));
        gfs->RemoveFile(gfsfilename);
        copyrs->OutputToMongoDB(gfs, gfsfilename);
        double stime = TimeCounting();
        FltIntRaster* mongors = FltIntRaster::Init(gfs, gfsfilename.c_str(),
                                                   false, maskrs_, false);
        cout << "Reading parameter finished, TIMESPAN " << ValueToString(TimeCounting() - stime) << " sec." << endl;
        // test mongors data
        EXPECT_EQ(4, mongors->GetCellNumber()); // m_nCells
        EXPECT_EQ(1, mongors->GetLayers());
        EXPECT_EQ(2, mongors->GetValidNumber());
        EXPECT_DOUBLE_EQ(4.5, mongors->GetAverage());
        // output to asc/tif file for comparison

        string newfullname4mongo = GetPathFromFullName(oldfullname) + "result" + SEP +
                newcorename + "_mongo." + GetSuffix(oldfullname);
        EXPECT_TRUE(rs_->OutputToFile(newfullname4mongo));
        EXPECT_TRUE(FileExists(newfullname4mongo));

        delete mongors;
        delete gfs;
    }
    //conn->Destroy(); // the MongoClient MUST not be destroyed or deleted!
    //delete conn;
#endif
    delete copyrs;
    // No need to release copyrs2 and copyrs3 by developers.
}

// calc_pos = True, use_mask_ext = False
TEST_P(clsRasterDataTestMaskWithin, CalPosNoMaskExt) {
    ASSERT_TRUE(rs_->ValidateRasterData());
    EXPECT_TRUE(rs_->Initialized());           // m_initialized
    EXPECT_FALSE(rs_->Is2DRaster());           // m_is2DRaster
    EXPECT_FALSE(rs_->MaskExtented());         // m_useMaskExtent
    EXPECT_FALSE(rs_->StatisticsCalculated()); // m_statisticsCalculated
    EXPECT_FALSE(rs_->PositionsCalculated());
    EXPECT_FALSE(rs_->PositionsAllocated());
    /// Calculate position data
    int valid_count;
    int** valid_positions = nullptr;
    rs_->GetRasterPositionData(&valid_count, &valid_positions);
    EXPECT_TRUE(rs_->PositionsCalculated());
    EXPECT_TRUE(rs_->PositionsAllocated());
    EXPECT_NE(nullptr, rs_->GetRasterDataPointer());         // m_rasterData
    EXPECT_EQ(nullptr, rs_->Get2DRasterDataPointer());       // m_raster2DData
    EXPECT_NE(nullptr, rs_->GetRasterPositionDataPointer()); // m_rasterPositionData

    /// Test members after constructing.
    EXPECT_EQ(2, rs_->GetDataLength()); // m_nCells, which will be nRows * nCols
    EXPECT_EQ(2, rs_->GetCellNumber()); // m_nCells

    EXPECT_FLOAT_EQ(-9999.f, rs_->GetNoDataValue());  // m_noDataValue
    EXPECT_FLOAT_EQ(-9999.f, rs_->GetDefaultValue()); // m_defaultValue

    // m_filePathName depends on the path of build, so no need to test.
    EXPECT_EQ(Corename, rs_->GetCoreName()); // m_coreFileName

    /** Get metadata, m_headers **/
    map<string, double> header_info = rs_->GetRasterHeader();
    EXPECT_EQ(CVT_INT(header_info.at(HEADER_RS_LAYERS)), rs_->GetLayers());
    EXPECT_EQ(CVT_INT(header_info.at(HEADER_RS_CELLSNUM)), rs_->GetCellNumber());

    EXPECT_EQ(2, rs_->GetRows());
    EXPECT_EQ(2, rs_->GetCols());
    EXPECT_DOUBLE_EQ(3., rs_->GetXllCenter());
    EXPECT_DOUBLE_EQ(3., rs_->GetYllCenter());
    EXPECT_DOUBLE_EQ(2., rs_->GetCellWidth());
    EXPECT_EQ(1, rs_->GetLayers());
    EXPECT_EQ("", rs_->GetSrsString());

    /** Calc and get basic statistics, m_statsMap **/
    EXPECT_EQ(2, rs_->GetValidNumber());
    EXPECT_DOUBLE_EQ(4., rs_->GetMinimum());
    EXPECT_DOUBLE_EQ(5., rs_->GetMaximum());
    EXPECT_DOUBLE_EQ(4.5, rs_->GetAverage());
    EXPECT_DOUBLE_EQ(0.5, rs_->GetStd());
    EXPECT_DOUBLE_EQ(1., rs_->GetRange());
    EXPECT_TRUE(rs_->StatisticsCalculated());

    EXPECT_NE(nullptr, rs_->GetMask()); // m_mask

    /** Test getting raster data **/
    int ncells = 0;
    float* rs_data = nullptr;
    EXPECT_TRUE(rs_->GetRasterData(&ncells, &rs_data)); // m_rasterData
    EXPECT_EQ(2, ncells);
    EXPECT_NE(nullptr, rs_data);
    EXPECT_FLOAT_EQ(4.f, rs_data[0]);
    EXPECT_FLOAT_EQ(5.f, rs_data[1]);

    float** rs_2ddata = nullptr;
    int nlyrs = -1;
    EXPECT_FALSE(rs_->Get2DRasterData(&ncells, &nlyrs, &rs_2ddata)); // m_raster2DData
    EXPECT_EQ(-1, ncells);
    EXPECT_EQ(-1, nlyrs);
    EXPECT_EQ(nullptr, rs_2ddata);

    /** Get raster cell value by various way **/
    /// Get cell value by index of raster_data_
    EXPECT_FLOAT_EQ(-9999.f, rs_->GetValueByIndex(-1));
    EXPECT_FLOAT_EQ(4.f, rs_->GetValueByIndex(0));
    EXPECT_FLOAT_EQ(5.f, rs_->GetValueByIndex(1));
    EXPECT_FLOAT_EQ(-9999.f, rs_->GetValueByIndex(2));
    EXPECT_FLOAT_EQ(-9999.f, rs_->GetValueByIndex(3));

    /// Get cell value by index of raster_data_/raster_2d_, compatible way
    int tmp_lyr = rs_->GetLayers();
    float* tmp_values = nullptr;
    Initialize1DArray(tmp_lyr, tmp_values, -9999.f);
    rs_->GetValueByIndex(-1, tmp_values); // index exceed, tmp_values will be released
    EXPECT_EQ(nullptr, tmp_values);
    rs_->GetValueByIndex(0, tmp_values); // nullptr input, tmp_values will be initialized
    EXPECT_NE(nullptr, tmp_values);
    EXPECT_FLOAT_EQ(4.f, tmp_values[0]);
    rs_->GetValueByIndex(1, tmp_values);
    EXPECT_NE(nullptr, tmp_values);
    EXPECT_FLOAT_EQ(5.f, tmp_values[0]);

    /// Get cell value by (row, col)
    EXPECT_FLOAT_EQ(-9999.f, rs_->GetValue(-1, 0));
    EXPECT_FLOAT_EQ(-9999.f, rs_->GetValue(0, 0));
    EXPECT_FLOAT_EQ(4.f, rs_->GetValue(0, 1));
    EXPECT_FLOAT_EQ(5.f, rs_->GetValue(1, 0));
    EXPECT_FLOAT_EQ(-9999.f, rs_->GetValue(1, 1));

    /// Get cell values by (row, col, *&array)
    rs_->GetValue(-1, 0, tmp_values);
    EXPECT_EQ(nullptr, tmp_values);
    rs_->GetValue(0, -1, tmp_values);
    EXPECT_EQ(nullptr, tmp_values);
    rs_->GetValue(0, 0, tmp_values);
    EXPECT_NE(nullptr, tmp_values);
    EXPECT_FLOAT_EQ(-9999.f, tmp_values[0]);
    rs_->GetValue(0, 1, tmp_values);
    EXPECT_NE(nullptr, tmp_values);
    EXPECT_FLOAT_EQ(4.f, tmp_values[0]);
    rs_->GetValue(1, 0, tmp_values);
    EXPECT_NE(nullptr, tmp_values);
    EXPECT_FLOAT_EQ(5.f, tmp_values[0]);

    Release1DArray(tmp_values);
    EXPECT_EQ(nullptr, tmp_values);

    // Get position index
    EXPECT_EQ(-2, rs_->GetPosition(-1., -2.));
    EXPECT_EQ(-2, rs_->GetPosition(-1., 9.));
    EXPECT_EQ(-2, rs_->GetPosition(1., 7.));
    EXPECT_EQ(-2, rs_->GetPosition(3.99, 7.05));
    EXPECT_EQ(-2, rs_->GetPosition(5.01, 7.00));
    EXPECT_EQ(-2, rs_->GetPosition(0.01, 5.5));
    EXPECT_EQ(-1, rs_->GetPosition(2.01, 5.1)); // NoData
    EXPECT_EQ(0, rs_->GetPosition(4.5, 5.5));
    EXPECT_EQ(-2, rs_->GetPosition(1., 3.));
    EXPECT_EQ(1, rs_->GetPosition(3., 3.));
    EXPECT_EQ(-1, rs_->GetPosition(5., 3.)); // NoData
    EXPECT_EQ(-2, rs_->GetPosition(1., 1.));
    EXPECT_EQ(-2, rs_->GetPosition(3., 1.));
    EXPECT_EQ(-2, rs_->GetPosition(5., 1.));
}

// calc_pos = True, use_mask_ext = True
TEST_P(clsRasterDataTestMaskWithin, CalPosUseMaskExt) {
    ASSERT_TRUE(rs_->ValidateRasterData());
    EXPECT_TRUE(rs_->Initialized());           // m_initialized
    EXPECT_FALSE(rs_->Is2DRaster());           // m_is2DRaster
    EXPECT_FALSE(rs_->MaskExtented());         // m_useMaskExtent
    EXPECT_FALSE(rs_->StatisticsCalculated()); // m_statisticsCalculated
    EXPECT_FALSE(rs_->PositionsCalculated());
    EXPECT_FALSE(rs_->PositionsAllocated());
    /// Calculate position data
    int valid_count;
    int** valid_positions = nullptr;
    rs_->GetRasterPositionData(&valid_count, &valid_positions);
    EXPECT_TRUE(rs_->PositionsCalculated());
    EXPECT_TRUE(rs_->PositionsAllocated());
    EXPECT_NE(nullptr, rs_->GetRasterDataPointer());         // m_rasterData
    EXPECT_EQ(nullptr, rs_->Get2DRasterDataPointer());       // m_raster2DData
    EXPECT_NE(nullptr, rs_->GetRasterPositionDataPointer()); // m_rasterPositionData
    /// Set to use the extent of mask data
    rs_->SetUseMaskExt();

    /// Test members after constructing.
    EXPECT_EQ(2, rs_->GetDataLength()); // m_nCells, which will be nRows * nCols
    EXPECT_EQ(2, rs_->GetCellNumber()); // m_nCells

    EXPECT_FLOAT_EQ(-9999.f, rs_->GetNoDataValue());  // m_noDataValue
    EXPECT_FLOAT_EQ(-9999.f, rs_->GetDefaultValue()); // m_defaultValue

    // m_filePathName depends on the path of build, so no need to test.
    EXPECT_EQ(Corename, rs_->GetCoreName()); // m_coreFileName

    /** Get metadata, m_headers **/
    map<string, double> header_info = rs_->GetRasterHeader();
    EXPECT_EQ(CVT_INT(header_info.at(HEADER_RS_LAYERS)), rs_->GetLayers());
    EXPECT_EQ(CVT_INT(header_info.at(HEADER_RS_CELLSNUM)), rs_->GetCellNumber());

    EXPECT_EQ(2, rs_->GetRows());
    EXPECT_EQ(2, rs_->GetCols());
    EXPECT_DOUBLE_EQ(3., rs_->GetXllCenter());
    EXPECT_DOUBLE_EQ(3., rs_->GetYllCenter());
    EXPECT_DOUBLE_EQ(2., rs_->GetCellWidth());
    EXPECT_EQ(1, rs_->GetLayers());
    EXPECT_EQ("", rs_->GetSrsString());

    /** Calc and get basic statistics, m_statsMap **/
    EXPECT_EQ(2, rs_->GetValidNumber());
    EXPECT_DOUBLE_EQ(4., rs_->GetMinimum());
    EXPECT_DOUBLE_EQ(5., rs_->GetMaximum());
    EXPECT_DOUBLE_EQ(4.5, rs_->GetAverage());
    EXPECT_DOUBLE_EQ(0.5, rs_->GetStd());
    EXPECT_DOUBLE_EQ(1., rs_->GetRange());
    EXPECT_TRUE(rs_->StatisticsCalculated());

    EXPECT_NE(nullptr, rs_->GetMask()); // m_mask

    /** Test getting raster data **/
    int ncells = 0;
    float* rs_data = nullptr;
    EXPECT_TRUE(rs_->GetRasterData(&ncells, &rs_data)); // m_rasterData
    EXPECT_EQ(2, ncells);
    EXPECT_NE(nullptr, rs_data);
    EXPECT_FLOAT_EQ(4.f, rs_data[0]);
    EXPECT_FLOAT_EQ(5.f, rs_data[1]);

    float** rs_2ddata = nullptr;
    int nlyrs = -1;
    EXPECT_FALSE(rs_->Get2DRasterData(&ncells, &nlyrs, &rs_2ddata)); // m_raster2DData
    EXPECT_EQ(-1, ncells);
    EXPECT_EQ(-1, nlyrs);
    EXPECT_EQ(nullptr, rs_2ddata);

    /** Get raster cell value by various way **/
    /// Get cell value by index of raster_data_
    EXPECT_FLOAT_EQ(-9999.f, rs_->GetValueByIndex(-1));
    EXPECT_FLOAT_EQ(4.f, rs_->GetValueByIndex(0));
    EXPECT_FLOAT_EQ(5.f, rs_->GetValueByIndex(1));
    EXPECT_FLOAT_EQ(-9999.f, rs_->GetValueByIndex(2));
    EXPECT_FLOAT_EQ(-9999.f, rs_->GetValueByIndex(3));

    /// Get cell value by index of raster_data_/raster_2d_, compatible way
    int tmp_lyr = rs_->GetLayers();
    float* tmp_values = nullptr;
    Initialize1DArray(tmp_lyr, tmp_values, -9999.f);
    rs_->GetValueByIndex(-1, tmp_values); // index exceed, tmp_values will be released
    EXPECT_EQ(nullptr, tmp_values);
    rs_->GetValueByIndex(0, tmp_values); // nullptr input, tmp_values will be initialized
    EXPECT_NE(nullptr, tmp_values);
    EXPECT_FLOAT_EQ(4.f, tmp_values[0]);
    rs_->GetValueByIndex(1, tmp_values);
    EXPECT_NE(nullptr, tmp_values);
    EXPECT_FLOAT_EQ(5.f, tmp_values[0]);

    /// Get cell value by (row, col)
    EXPECT_FLOAT_EQ(-9999.f, rs_->GetValue(-1, 0));
    EXPECT_FLOAT_EQ(-9999.f, rs_->GetValue(0, 0));
    EXPECT_FLOAT_EQ(4.f, rs_->GetValue(0, 1));
    EXPECT_FLOAT_EQ(5.f, rs_->GetValue(1, 0));
    EXPECT_FLOAT_EQ(-9999.f, rs_->GetValue(1, 1));

    /// Get cell values by (row, col, *&array)
    rs_->GetValue(-1, 0, tmp_values);
    EXPECT_EQ(nullptr, tmp_values);
    rs_->GetValue(0, -1, tmp_values);
    EXPECT_EQ(nullptr, tmp_values);
    rs_->GetValue(0, 0, tmp_values);
    EXPECT_NE(nullptr, tmp_values);
    EXPECT_FLOAT_EQ(-9999.f, tmp_values[0]);
    rs_->GetValue(0, 1, tmp_values);
    EXPECT_NE(nullptr, tmp_values);
    EXPECT_FLOAT_EQ(4.f, tmp_values[0]);
    rs_->GetValue(1, 0, tmp_values);
    EXPECT_NE(nullptr, tmp_values);
    EXPECT_FLOAT_EQ(5.f, tmp_values[0]);

    Release1DArray(tmp_values);
    EXPECT_EQ(nullptr, tmp_values);

    // Get position index
    EXPECT_EQ(-2, rs_->GetPosition(-1., -2.));
    EXPECT_EQ(-2, rs_->GetPosition(-1., 9.));
    EXPECT_EQ(-2, rs_->GetPosition(1., 7.));
    EXPECT_EQ(-2, rs_->GetPosition(3.99, 7.05));
    EXPECT_EQ(-2, rs_->GetPosition(5.01, 7.00));
    EXPECT_EQ(-2, rs_->GetPosition(0.01, 5.5));
    EXPECT_EQ(-1, rs_->GetPosition(2.01, 5.1)); // NoData
    EXPECT_EQ(0, rs_->GetPosition(4.5, 5.5));
    EXPECT_EQ(-2, rs_->GetPosition(1., 3.));
    EXPECT_EQ(1, rs_->GetPosition(3., 3.));
    EXPECT_EQ(-1, rs_->GetPosition(5., 3.)); // NoData
    EXPECT_EQ(-2, rs_->GetPosition(1., 1.));
    EXPECT_EQ(-2, rs_->GetPosition(3., 1.));
    EXPECT_EQ(-2, rs_->GetPosition(5., 1.));
}


#ifdef USE_GDAL
INSTANTIATE_TEST_CASE_P(SingleLayer, clsRasterDataTestMaskWithin,
                        Values(new InputRasterFiles(AscFile, MaskAscFileS),
                            new InputRasterFiles(TifFile, MaskTifFileS)));
#else
INSTANTIATE_TEST_CASE_P(SingleLayer, clsRasterDataTestMaskWithin,
                        Values(new InputRasterFiles(AscFile, MaskAscFileS)));
#endif /* USE_GDAL */

} /* namespace */
