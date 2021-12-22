/*!
 * \brief Test description
 *
 *        TEST CASE NAME (or TEST SUITE):
 *            clsRasterData2DSplit: Split 2D raster with mask by the subset feature of clsRasterData.
 *
 *        Since we mainly support ASC and GDAL(e.g., TIFF),
 *        value-parameterized tests of Google Test will be used.
 * \cite https://github.com/google/googletest/blob/master/googletest/samples/sample7_unittest.cc
 * \version 1.0
 * \authors Liangjun Zhu (zlj@lreis.ac.cn)
 * \revised 2021-12-12 - lj - Original version.
 *
 */
#include "gtest/gtest.h"
#include "../../src/data_raster.hpp"
#include "../../src/utils_array.h"
#include "../../src/utils_string.h"
#include "../../src/utils_filesystem.h"

using namespace ccgl::data_raster;
using namespace ccgl::utils_array;
using namespace ccgl::utils_string;
using namespace ccgl::utils_filesystem;

namespace {
using testing::TestWithParam;
using testing::Values;

string Rspath = GetAppPath() + "./data/raster/";
string Dstpath = Rspath + "result/";

string maskname = "tinydemo_raster_r4c7";
string corename = "tinydemo_raster_r5c8";

string rs1_asc = Rspath + corename + ".asc";
string rs2_asc = Rspath + corename + "_2.asc";
string rs3_asc = Rspath + corename + "_3.asc";

string rs1_tif = Rspath + corename + ".tif";
string rs2_tif = Rspath + corename + "_2.tif";
string rs3_tif = Rspath + corename + "_3.tif";

string mask_asc_file = Rspath + maskname + ".asc";
string mask_tif_file = Rspath + maskname + ".tif";

struct InputRasterFiles {
public:
    InputRasterFiles(const string& rs1, const string& rs2,
                     const string& rs3, const string& maskf) {
        raster_name1 = rs1.c_str();
        raster_name2 = rs2.c_str();
        raster_name3 = rs3.c_str();
        mask_name = maskf.c_str();
    }
    const char* raster_name1;
    const char* raster_name2;
    const char* raster_name3;
    const char* mask_name;
};

//Inside the test body, fixture constructor, SetUp(), and TearDown() you
//can refer to the test parameter by GetParam().  In this case, the test
//parameter is a factory function which we call in fixture's SetUp() to
//create and store an instance of clsRasterData<float>.
class clsRasterData2DSplitMerge : public TestWithParam<InputRasterFiles*> {
public:
    clsRasterData2DSplitMerge() : maskrs_(nullptr) {
    }

    virtual ~clsRasterData2DSplitMerge() { delete maskrs_; }

    void SetUp() OVERRIDE {
        maskrs_ = IntRaster::Init(GetParam()->mask_name);
        ASSERT_NE(nullptr, maskrs_);

        EXPECT_TRUE(MakeDirectory(Dstpath));
    }

    void TearDown() OVERRIDE {
        delete maskrs_;
        maskrs_ = nullptr;
    }

protected:
    IntRaster* maskrs_;
};

TEST_P(clsRasterData2DSplitMerge, RasterIO) {
    EXPECT_FALSE(maskrs_->PositionsCalculated());
    EXPECT_TRUE(maskrs_->BuildSubSet());
    EXPECT_TRUE(maskrs_->PositionsCalculated());
    map<int, SubsetPositions*> subset = maskrs_->GetSubset();
    int n_subset = CVT_INT(subset.size());
    EXPECT_EQ(n_subset, 4);
    SubsetPositions* sub1 = subset.at(1);
    EXPECT_EQ(sub1->g_srow, 0);
    EXPECT_EQ(sub1->g_erow, 3);
    EXPECT_EQ(sub1->g_scol, 0);
    EXPECT_EQ(sub1->g_ecol, 2);
    EXPECT_EQ(sub1->n_cells, 7);
    EXPECT_EQ(sub1->global_[0], 0);
    EXPECT_EQ(sub1->global_[1], 2);
    EXPECT_EQ(sub1->global_[2], 3);
    EXPECT_EQ(sub1->global_[3], 7);
    EXPECT_EQ(sub1->global_[4], 8);
    EXPECT_EQ(sub1->global_[5], 9);
    EXPECT_EQ(sub1->global_[6], 14);
    SubsetPositions* sub2 = subset.at(2);
    EXPECT_EQ(sub2->g_srow, 1);
    EXPECT_EQ(sub2->g_erow, 3);
    EXPECT_EQ(sub2->g_scol, 2);
    EXPECT_EQ(sub2->g_ecol, 3);
    EXPECT_EQ(sub2->n_cells, 4);
    EXPECT_EQ(sub2->global_[0], 4);
    EXPECT_EQ(sub2->global_[1], 5);
    EXPECT_EQ(sub2->global_[2], 10);
    EXPECT_EQ(sub2->global_[3], 15);
    SubsetPositions* sub3 = subset.at(3);
    EXPECT_EQ(sub3->g_srow, 0);
    EXPECT_EQ(sub3->g_erow, 3);
    EXPECT_EQ(sub3->g_scol, 3);
    EXPECT_EQ(sub3->g_ecol, 6);
    EXPECT_EQ(sub3->n_cells, 6);
    EXPECT_EQ(sub3->global_[0], 1);
    EXPECT_EQ(sub3->global_[1], 6);
    EXPECT_EQ(sub3->global_[2], 11);
    EXPECT_EQ(sub3->global_[3], 16);
    EXPECT_EQ(sub3->global_[4], 17);
    EXPECT_EQ(sub3->global_[5], 18);
    SubsetPositions* sub4 = subset.at(4);
    EXPECT_EQ(sub4->g_srow, 2);
    EXPECT_EQ(sub4->g_erow, 3);
    EXPECT_EQ(sub4->g_scol, 5);
    EXPECT_EQ(sub4->g_ecol, 6);
    EXPECT_EQ(sub4->n_cells, 3);
    EXPECT_EQ(sub4->global_[0], 12);
    EXPECT_EQ(sub4->global_[1], 13);
    EXPECT_EQ(sub4->global_[2], 19);

    map<int, int> new_group;
    new_group[1] = 1;
    new_group[2] = 2;
    new_group[3] = 3;
    new_group[4] = 1;
    EXPECT_TRUE(maskrs_->RebuildSubSet(new_group));
    map<int, SubsetPositions*> newsubset = maskrs_->GetSubset();
    n_subset = CVT_INT(newsubset.size());
    EXPECT_EQ(n_subset, 3);
    SubsetPositions* newsub1 = newsubset.at(1);
    EXPECT_EQ(newsub1->g_srow, 0);
    EXPECT_EQ(newsub1->g_erow, 3);
    EXPECT_EQ(newsub1->g_scol, 0);
    EXPECT_EQ(newsub1->g_ecol, 6);
    EXPECT_EQ(newsub1->n_cells, 10);
    EXPECT_EQ(newsub1->global_[0], 0);
    EXPECT_EQ(newsub1->global_[1], 2);
    EXPECT_EQ(newsub1->global_[2], 3);
    EXPECT_EQ(newsub1->global_[3], 7);
    EXPECT_EQ(newsub1->global_[4], 8);
    EXPECT_EQ(newsub1->global_[5], 9);
    EXPECT_EQ(newsub1->global_[6], 12);
    EXPECT_EQ(newsub1->global_[7], 13);
    EXPECT_EQ(newsub1->global_[8], 14);
    EXPECT_EQ(newsub1->global_[9], 19);
    SubsetPositions* newsub2 = newsubset.at(2);
    EXPECT_EQ(newsub2->g_srow, 1);
    EXPECT_EQ(newsub2->g_erow, 3);
    EXPECT_EQ(newsub2->g_scol, 2);
    EXPECT_EQ(newsub2->g_ecol, 3);
    EXPECT_EQ(newsub2->n_cells, 4);
    EXPECT_EQ(newsub2->global_[0], 4);
    EXPECT_EQ(newsub2->global_[1], 5);
    EXPECT_EQ(newsub2->global_[2], 10);
    EXPECT_EQ(newsub2->global_[3], 15);
    SubsetPositions* newsub3 = newsubset.at(3);
    EXPECT_EQ(newsub3->g_srow, 0);
    EXPECT_EQ(newsub3->g_erow, 3);
    EXPECT_EQ(newsub3->g_scol, 3);
    EXPECT_EQ(newsub3->g_ecol, 6);
    EXPECT_EQ(newsub3->n_cells, 6);
    EXPECT_EQ(newsub3->global_[0], 1);
    EXPECT_EQ(newsub3->global_[1], 6);
    EXPECT_EQ(newsub3->global_[2], 11);
    EXPECT_EQ(newsub3->global_[3], 16);
    EXPECT_EQ(newsub3->global_[4], 17);
    EXPECT_EQ(newsub3->global_[5], 18);

    /** Set new data of subset and output to new files **/
    map<int, float*> newdata = map<int, float*>();
    float* data1 = nullptr;
    Initialize1DArray(newsub1->n_cells, data1, 1.f);
    data1[0] = 2008.f;
    data1[1] = 11.f;
    data1[2] = 9.f;
    data1[7] = 2017.f;
    data1[8] = 5.f;
    data1[9] = 1.f;
    newsub1->SetData(newsub1->n_cells, data1);
    newdata[1] = data1;

    float* data2 = nullptr;
    Initialize1DArray(newsub2->n_cells, data2, 2.f);
    data2[0] = 2017.f;
    data2[1] = 1.f;
    data2[2] = 7.f;
    newsub2->SetData(newsub2->n_cells, data2);
    newdata[2] = data2;

    float* data3 = nullptr;
    Initialize1DArray(newsub3->n_cells, data3, 3.f);
    data3[0] = 2019.f;
    data3[1] = 2.f;
    data3[2] = 18.f;
    newsub3->SetData(newsub3->n_cells, data3);
    newdata[3] = data3;

    /** Output subset to new files **/
    EXPECT_TRUE(maskrs_->OutputSubsetToFile(false, false, Dstpath));
    string suffix = GetSuffix(GetParam()->mask_name);
    map<int, SubsetPositions*> subsets = maskrs_->GetSubset();
    for (auto it = subsets.begin(); it != subsets.end(); ++it) {
        string outfile = Dstpath;
        outfile += maskname;
        outfile += "_";
        outfile += itoa(it->first);
        outfile += "_";
        outfile += ".";
        outfile += suffix;
        EXPECT_TRUE(FileExists(outfile));
        float* tmp = newdata.at(it->first);
        FltRaster* tmp_rs = FltRaster::Init(outfile, true);
        EXPECT_FALSE(nullptr == tmp_rs);
        EXPECT_TRUE(tmp_rs->PositionsCalculated());
        int len;
        float* validdata = nullptr;
        tmp_rs->GetRasterData(&len, &validdata);
        for (int k = 0; k < len; k++) {
            EXPECT_FLOAT_EQ(validdata[k], tmp[k]);
        }
        delete tmp_rs;
    }

    /** Output raster data according to subset's data **/
    string outfile = Dstpath;
    outfile += maskname;
    outfile += "_merge_subset.";
    outfile += suffix;
    maskrs_->OutputToFile(outfile, true);

    /** Read the output raster data and check **/
    FltRaster* newrs = FltRaster::Init(outfile, true);
    EXPECT_NE(nullptr, newrs);
    EXPECT_EQ(20, newrs->GetCellNumber());
    EXPECT_FLOAT_EQ(newrs->GetValueByIndex(0), 2008.f);
    EXPECT_FLOAT_EQ(newrs->GetValueByIndex(1), 2019.f);
    EXPECT_FLOAT_EQ(newrs->GetValueByIndex(2), 11.f);
    EXPECT_FLOAT_EQ(newrs->GetValueByIndex(3), 9.f);
    EXPECT_FLOAT_EQ(newrs->GetValueByIndex(4), 2017.f);
    EXPECT_FLOAT_EQ(newrs->GetValueByIndex(5), 1.f);
    EXPECT_FLOAT_EQ(newrs->GetValueByIndex(6), 2.f);
    EXPECT_FLOAT_EQ(newrs->GetValueByIndex(7), 1.f);
    EXPECT_FLOAT_EQ(newrs->GetValueByIndex(8), 1.f);
    EXPECT_FLOAT_EQ(newrs->GetValueByIndex(9), 1.f);
    EXPECT_FLOAT_EQ(newrs->GetValueByIndex(10), 7.f);
    EXPECT_FLOAT_EQ(newrs->GetValueByIndex(11), 18.f);
    EXPECT_FLOAT_EQ(newrs->GetValueByIndex(12), 1.f);
    EXPECT_FLOAT_EQ(newrs->GetValueByIndex(13), 2017.f);
    EXPECT_FLOAT_EQ(newrs->GetValueByIndex(14), 5.f);
    EXPECT_FLOAT_EQ(newrs->GetValueByIndex(15), 2.f);
    EXPECT_FLOAT_EQ(newrs->GetValueByIndex(16), 3.f);
    EXPECT_FLOAT_EQ(newrs->GetValueByIndex(17), 3.f);
    EXPECT_FLOAT_EQ(newrs->GetValueByIndex(18), 3.f);
    EXPECT_FLOAT_EQ(newrs->GetValueByIndex(19), 1.f);

    delete newrs;

    // release newdata
    for (auto it = newdata.begin(); it != newdata.end(); ) {
        Release1DArray(it->second);
        newdata.erase(it++);
    }
}



#ifdef USE_GDAL
    INSTANTIATE_TEST_CASE_P(MultiLayers, clsRasterData2DSplitMerge,
                            Values(new InputRasterFiles(rs1_asc, rs2_asc, rs3_asc, mask_asc_file),
                                new InputRasterFiles(rs1_tif, rs2_tif, rs3_tif, mask_tif_file)));
#else
    INSTANTIATE_TEST_CASE_P(MultiLayers, clsRasterData2DSplitMerge,
                            Values(new InputRasterFiles(rs1_asc, rs2_asc, rs3_asc, mask_asc_file)));
#endif /* USE_GDAL */

} /* namespace */
