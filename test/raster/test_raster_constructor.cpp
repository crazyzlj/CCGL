/*!
 * \brief Test clsRasterData of blank constructor to make sure no exception thrown.
 *
 * \version 1.3
 * \authors Liangjun Zhu (zlj@lreis.ac.cn)
 * \revised 2017-12-02 - lj - Original version.
 *          2018-05-03 - lj - Integrated into CCGL.
 *          2021-07-20 - lj - Update after changes of GetValue and GetValueByIndex.
 *          2021-11-27 - lj - Add more tests.
 *
 */
#include "gtest/gtest.h"

#include "../../src/data_raster.hpp"
#include "../../src/utils_filesystem.h"
#include "../../src/utils_array.h"

using namespace ccgl::data_raster;
using namespace ccgl::utils_array;
using namespace ccgl::utils_filesystem;


namespace {
string datapath = GetAppPath() + "./data/raster/";

string not_existed_rs = datapath + "not_existed_rs.tif";
string not_std_asc = datapath + "tinydemo_not-std-asc_r2c2.asc";

string rs_mask = datapath + "mask_byte_r3c2";
string rs_byte = datapath + "byte_r3c3.tif";
string rs_byte_signed = datapath + "byte_signed_r3c3.tif";
string rs_byte_signed_noneg = datapath + "byte_signed_no-negative_r3c3.tif";
string rs_uint16 = datapath + "uint16_r3c3.tif";
string rs_int16 = datapath + "int16_r3c3.tif";
string rs_uint32 = datapath + "uint32_r3c3.tif";
string rs_int32 = datapath + "int32_r3c3.tif";
string rs_float = datapath + "float32_r3c3.tif";
string rs_double = datapath + "float64_r3c3.tif";

TEST(clsRasterDataTestBlankCtor, ValidateAccess) {
    /// 0. Create an clsRasterData instance with blank ctor
    clsRasterData<float, int>* rs = new clsRasterData<float, int>();
    /// 1. Test members after constructing.
    EXPECT_EQ(-1, rs->GetDataLength()); // m_nCells
    EXPECT_EQ(-1, rs->GetCellNumber()); // m_nCells

    EXPECT_FLOAT_EQ(-9999.f, rs->GetNoDataValue());  // m_noDataValue
    EXPECT_FLOAT_EQ(-9999.f, rs->GetDefaultValue()); // m_defaultValue

    EXPECT_EQ("", rs->GetFilePath()); // m_filePathName
    EXPECT_EQ("", rs->GetCoreName()); // m_coreFileName

    EXPECT_TRUE(rs->Initialized());           // m_initialized
    EXPECT_FALSE(rs->Is2DRaster());           // m_is2DRaster
    EXPECT_FALSE(rs->PositionsCalculated());  // m_calcPositions
    EXPECT_FALSE(rs->PositionsAllocated());   // m_storePositions
    EXPECT_FALSE(rs->MaskExtented());         // m_useMaskExtent
    EXPECT_FALSE(rs->StatisticsCalculated()); // m_statisticsCalculated

    EXPECT_FALSE(rs->ValidateRasterData());

    EXPECT_EQ(nullptr, rs->GetRasterDataPointer());         // m_rasterData
    EXPECT_EQ(nullptr, rs->Get2DRasterDataPointer());       // m_raster2DData
    EXPECT_EQ(nullptr, rs->GetRasterPositionDataPointer()); // m_rasterPositionData

    /** Get metadata, m_headers **/
    EXPECT_EQ(-9999, rs->GetRows());
    EXPECT_EQ(-9999, rs->GetCols());
    EXPECT_DOUBLE_EQ(-9999., rs->GetXllCenter());
    EXPECT_DOUBLE_EQ(-9999., rs->GetYllCenter());
    EXPECT_DOUBLE_EQ(-9999., rs->GetCellWidth());
    EXPECT_EQ(-1, rs->GetLayers());
    EXPECT_EQ("", rs->GetSrsString());

    /** Calc and get basic statistics, m_statsMap **/
    EXPECT_EQ(-9999, rs->GetValidNumber());
    EXPECT_DOUBLE_EQ(-9999., rs->GetMinimum());
    EXPECT_DOUBLE_EQ(-9999., rs->GetMaximum());
    EXPECT_DOUBLE_EQ(-9999., rs->GetAverage());
    EXPECT_DOUBLE_EQ(-9999., rs->GetStd());
    EXPECT_DOUBLE_EQ(-9999., rs->GetRange());
    EXPECT_FALSE(rs->StatisticsCalculated());

    EXPECT_EQ(nullptr, rs->GetMask()); // m_mask

    /** Test getting position data **/
    int ncells = -1;
    int** positions = nullptr;
    rs->GetRasterPositionData(&ncells, &positions); // m_rasterPositionData
    EXPECT_EQ(-1, ncells);
    EXPECT_EQ(nullptr, positions);

    /** Test getting raster data **/
    float* rs_data = nullptr;
    EXPECT_FALSE(rs->GetRasterData(&ncells, &rs_data)); // m_rasterData
    EXPECT_EQ(-1, ncells);
    EXPECT_EQ(nullptr, rs_data);

    float** rs_2ddata = nullptr;
    int nlyrs = -1;
    EXPECT_FALSE(rs->Get2DRasterData(&ncells, &nlyrs, &rs_2ddata)); // m_raster2DData
    EXPECT_EQ(-1, ncells);
    EXPECT_EQ(-1, nlyrs);
    EXPECT_EQ(nullptr, rs_2ddata);

    /** Get raster cell value by various way **/
    EXPECT_FLOAT_EQ(-9999.f, rs->GetValueByIndex(-1));
    EXPECT_FLOAT_EQ(-9999.f, rs->GetValueByIndex(0));
    EXPECT_FLOAT_EQ(-9999.f, rs->GetValueByIndex(540, 1));
    EXPECT_FLOAT_EQ(-9999.f, rs->GetValueByIndex(541, 1));
    EXPECT_FLOAT_EQ(-9999.f, rs->GetValueByIndex(29));
    EXPECT_FLOAT_EQ(-9999.f, rs->GetValueByIndex(29, 0));
    EXPECT_FLOAT_EQ(-9999.f, rs->GetValueByIndex(-1, 2));
    EXPECT_FLOAT_EQ(-9999.f, rs->GetValueByIndex(541, 2));

    int tmp_lyr = rs->GetLayers();
    float* tmp_values = nullptr;
    Initialize1DArray(tmp_lyr, tmp_values, -9999.f);
    rs->GetValueByIndex(-1, tmp_values);
    EXPECT_EQ(nullptr, tmp_values);
    rs->GetValueByIndex(0, tmp_values);
    EXPECT_EQ(nullptr, tmp_values);

    EXPECT_FLOAT_EQ(-9999.f, rs->GetValue(-1, 0));
    EXPECT_FLOAT_EQ(-9999.f, rs->GetValue(20, 0));
    EXPECT_FLOAT_EQ(-9999.f, rs->GetValue(0, -1));
    EXPECT_FLOAT_EQ(-9999.f, rs->GetValue(0, 30));
    EXPECT_FLOAT_EQ(-9999.f, rs->GetValue(2, 4, -1));
    EXPECT_FLOAT_EQ(-9999.f, rs->GetValue(2, 4, 2));
    EXPECT_FLOAT_EQ(-9999.f, rs->GetValue(2, 4));
    EXPECT_FLOAT_EQ(-9999.f, rs->GetValue(2, 4, 1));

    rs->GetValue(-1, 0, tmp_values);
    EXPECT_EQ(nullptr, tmp_values);
    rs->GetValue(0, -1, tmp_values);
    EXPECT_EQ(nullptr, tmp_values);
    rs->GetValue(0, 0, tmp_values);
    EXPECT_EQ(nullptr, tmp_values);
    rs->GetValue(0, 1, tmp_values);
    EXPECT_EQ(nullptr, tmp_values);

    Release1DArray(tmp_values);

    // Get position
    EXPECT_EQ(-2, rs->GetPosition(4.05f, 37.95f));
    EXPECT_EQ(-2, rs->GetPosition(5.95f, 36.05f));

    /** Set value **/

    // Set raster data value
    rs->SetValue(2, 4, 18.06f);
    EXPECT_FLOAT_EQ(-9999.f, rs->GetValue(2, 4));
    rs->SetValue(0, 0, 1.f);
    EXPECT_FLOAT_EQ(-9999.f, rs->GetValue(0, 0));

    /** Output to new file **/
    string newfullname = ccgl::utils_filesystem::GetAppPath() + SEP + "no_output.tif";
    EXPECT_FALSE(rs->OutputToFile(newfullname));

    delete rs;
}

TEST(clsRasterDataFailedConstructor, FailedCases) {
    FltIntRaster* noexisted_rs = FltIntRaster::Init(not_existed_rs);
    EXPECT_EQ(nullptr, noexisted_rs);

    vector<string> files;
    files.push_back(not_existed_rs);
    files.push_back(not_std_asc);
    noexisted_rs = FltIntRaster::Init(files);
    EXPECT_EQ(nullptr, noexisted_rs);
}

#ifdef USE_GDAL
TEST(clsRasterDataUnsignedByte, FullIO) {
    clsRasterData<unsigned char>* mask_rs = clsRasterData<unsigned char>::Init(rs_mask);
    clsRasterData<unsigned char>* byte_rs = clsRasterData<unsigned char>
            ::Init(rs_byte, true, mask_rs, true);

    // Situation 1:
    string rs_out1 = AppendCoreFileName(rs_byte, "masked");
    EXPECT_TRUE(byte_rs->GetDataType() == RDT_UByte);
    EXPECT_TRUE(byte_rs->GetOutDataType() == RDT_UByte);
    byte_rs->OutputToFile(rs_out1);
    EXPECT_TRUE(FileExists(rs_out1));
}
#endif

} /* namespace */
