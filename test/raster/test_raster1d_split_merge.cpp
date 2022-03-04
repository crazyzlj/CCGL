/*!
 * \brief Test description
 *
 *        TEST CASE NAME (or TEST SUITE):
 *            clsRasterDataSplitMerge: Split and merge raster with mask
 *                                     by the subset feature of clsRasterData.
 *
 *        Since we mainly support ASC and GDAL(e.g., TIFF),
 *        value-parameterized tests of Google Test will be used.
 * \cite https://github.com/google/googletest/blob/master/googletest/samples/sample7_unittest.cc
 * \version 1.0
 * \authors Liangjun Zhu, zlj(at)lreis.ac.cn; crazyzlj(at)gmail.com
 * \remarks 2021-12-12 - lj - Original version.
 *
 */
#include "gtest/gtest.h"
#include "../../src/data_raster.hpp"
#include "../../src/utils_array.h"
#include "../../src/utils_string.h"
#include "../../src/utils_filesystem.h"
#ifdef USE_MONGODB
#include "../../src/db_mongoc.h"
#endif
#include "../test_global.h"

using namespace ccgl::data_raster;
using namespace ccgl::utils_array;
using namespace ccgl::utils_string;
using namespace ccgl::utils_filesystem;
#ifdef USE_MONGODB
using namespace ccgl::db_mongoc;
#endif

extern GlobalEnvironment* GlobalEnv;

namespace {
using testing::TestWithParam;
using testing::Values;

string Rspath = GetAppPath() + "./data/raster/";
string Dstpath = Rspath + "result/";

string maskname = "tinydemo_raster_r4c7";
string corename = "tinydemo_raster_r5c8";

string rs1_asc = Rspath + corename + ".asc";
string rs1_tif = Rspath + corename + ".tif";

string mask_asc_file = Rspath + maskname + ".asc";
string mask_tif_file = Rspath + maskname + ".tif";

struct InputRasterFiles {
public:
    InputRasterFiles(const string& maskf, const string& rsf) {
        mask_name = maskf.c_str();
        raster_name = rsf.c_str();
    }
    const char* mask_name;
    const char* raster_name;
};

//Inside the test body, fixture constructor, SetUp(), and TearDown() you
//can refer to the test parameter by GetParam().  In this case, the test
//parameter is a factory function which we call in fixture's SetUp() to
//create and store an instance of clsRasterData<float>.
class clsRasterDataSplitMerge: public TestWithParam<InputRasterFiles *> {
public:
    clsRasterDataSplitMerge() : maskrs_(nullptr) {
    }

    virtual ~clsRasterDataSplitMerge() { ; }

    void SetUp() OVERRIDE {
        maskrs_ = IntRaster::Init(GetParam()->mask_name);
        ASSERT_NE(nullptr, maskrs_);

        EXPECT_TRUE(MakeDirectory(Dstpath));
    }

    void TearDown() OVERRIDE {
        delete maskrs_;
    }

protected:
    IntRaster* maskrs_;
};

TEST_P(clsRasterDataSplitMerge, MaskLyrIO) {
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
    EXPECT_TRUE(newsub1->SetData(newsub1->n_cells, data1));
    newdata[1] = data1;

    float* data2 = nullptr;
    Initialize1DArray(newsub2->n_cells, data2, 2.f);
    data2[0] = 2017.f;
    data2[1] = 1.f;
    data2[2] = 7.f;
    EXPECT_TRUE(newsub2->SetData(newsub2->n_cells, data2));
    newdata[2] = data2;

    float* data3 = nullptr;
    Initialize1DArray(newsub3->n_cells, data3, 3.f);
    data3[0] = 2019.f;
    data3[1] = 2.f;
    data3[2] = 18.f;
    EXPECT_TRUE(newsub3->SetData(newsub3->n_cells, data3));
    newdata[3] = data3;

    /** Output subset to new files **/
    EXPECT_TRUE(maskrs_->OutputSubsetToFile(false, false, Dstpath));
    string suffix = GetSuffix(GetParam()->mask_name);
    map<int, SubsetPositions*>& subsets = maskrs_->GetSubset();
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

#ifdef USE_MONGODB
    /** Output subset data to MongoDB **/
    string gfsfilename = maskrs_->GetCoreName() + "_" + GetSuffix(maskrs_->GetFilePath());
    gfsfilename += "_subset";
    EXPECT_TRUE(maskrs_->OutputSubsetToMongoDB(GlobalEnv->gfs_, gfsfilename, ccgl::STRING_MAP(),
                    false, false, true)); // store fullsize data

    EXPECT_TRUE(maskrs_->OutputSubsetToMongoDB(GlobalEnv->gfs_, gfsfilename, ccgl::STRING_MAP(),
                    false, false, false)); // store valid data only

    IntRaster* maskrs4mongodata = new IntRaster(maskrs_);
    IntRaster* maskrs4mongodata2 = new IntRaster(maskrs_);
    map<int, SubsetPositions*>& subsetsfull = maskrs4mongodata->GetSubset();
    map<int, SubsetPositions*>& subsetsvalid = maskrs4mongodata2->GetSubset();

    for (auto it = subsetsfull.begin(); it != subsetsfull.end(); ++it) {
        string gfsfull = gfsfilename + "_" + itoa(it->first);
        EXPECT_TRUE(it->second->ReadFromMongoDB(GlobalEnv->gfs_, gfsfull));
    }
    for (auto it = subsetsvalid.begin(); it != subsetsvalid.end(); ++it) {
        string gfsvalid = gfsfilename + "_valid_" + itoa(it->first);
        EXPECT_TRUE(it->second->ReadFromMongoDB(GlobalEnv->gfs_, gfsvalid));
    }

    // check consistent of valid values loaded in gfsfull and gfsvalid
    for (auto it = subsetsfull.begin(); it != subsetsfull.end(); ++it) {
        SubsetPositions* full = it->second;
        SubsetPositions* valid = subsetsvalid.at(it->first);
        EXPECT_EQ(full->n_lyrs, valid->n_lyrs);
        EXPECT_EQ(full->alloc_, valid->alloc_);
        EXPECT_EQ(full->n_cells, valid->n_cells);
        EXPECT_EQ(full->g_srow, valid->g_srow);
        EXPECT_EQ(full->g_erow, valid->g_erow);
        EXPECT_EQ(full->g_scol, valid->g_scol);
        EXPECT_EQ(full->g_ecol, valid->g_ecol);
        for (int i = 0; i < full->n_cells; i++) {
            EXPECT_EQ(full->local_pos_[i][0], valid->local_pos_[i][0]);
            EXPECT_EQ(full->local_pos_[i][1], valid->local_pos_[i][1]);
            EXPECT_DOUBLE_EQ(full->data_[i], valid->data_[i]);
        }
    }

    delete maskrs4mongodata;
    delete maskrs4mongodata2;

#endif


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
    for (auto it = newdata.begin(); it != newdata.end(); ++it) {
        Release1DArray(it->second);
    }
    newdata.clear();
}

// read raster based on mask layer which has several subset
//   output raster data according to mask's subset
//   read subset data to combine an entire raster data
TEST_P(clsRasterDataSplitMerge, SplitRaster) {
    // prepare mask data with user-specified groups
    EXPECT_FALSE(maskrs_->PositionsCalculated());
    map<int, int> new_group;
    new_group[1] = 1;
    new_group[2] = 2;
    new_group[3] = 3;
    new_group[4] = 1;
    EXPECT_TRUE(maskrs_->BuildSubSet(new_group));
    EXPECT_TRUE(maskrs_->PositionsCalculated());
    // read raster data with mask
    FltIntRaster* rs = FltIntRaster::Init(GetParam()->raster_name,
                                          true, maskrs_, true);
    EXPECT_NE(nullptr, rs);
    
    map<int, SubsetPositions*>& subset = maskrs_->GetSubset();
    map<int, SubsetPositions*>& rs_subset = rs->GetSubset();
    EXPECT_FALSE(subset.empty());
    EXPECT_FALSE(rs_subset.empty());
    EXPECT_EQ(subset.size(), 3);
    EXPECT_EQ(rs_subset.size(), 2);

    /** Output subset to new files **/
    EXPECT_TRUE(rs->OutputSubsetToFile(true, false, Dstpath));
    /** Check the output files **/
    map<int, float*> subarray = map<int, float*>();
    float* data1 = nullptr;
    Initialize1DArray(10, data1, -9999.f);
    data1[0] = 1.1f;
    data1[1] = 4.4f;
    data1[2] = 5.5f;
    data1[3] = 10.f;
    data1[4] = 11.11f;
    data1[5] = 12.12f;
    data1[6] = 15.15f;
    data1[7] = 16.16f;
    data1[8] = 17.17f;
    data1[9] = 22.22f;
    subarray[1] = data1;

    float* data3 = nullptr;
    Initialize1DArray(6, data3, -9999.f);
    data3[0] = 2.2f;
    data3[1] = 8.8f;
    data3[2] = 14.14f;
    data3[3] = 19.19f;
    data3[4] =20.2f;
    data3[5] = 21.21f;
    subarray[3] = data3;

    string suffix = GetSuffix(GetParam()->raster_name);
    for (auto it = rs_subset.begin(); it != rs_subset.end(); ++it) {
        string outfile = Dstpath;
        outfile += corename;
        outfile += "_";
        outfile += itoa(it->first);
        outfile += "_";
        outfile += ".";
        outfile += suffix;
        EXPECT_TRUE(FileExists(outfile));
        float* tmp = subarray.at(it->first);
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
    for (auto it = subarray.begin(); it != subarray.end(); ++it) {
        Release1DArray(it->second);
    }
    subarray.clear();
    delete rs;
}


#ifdef USE_GDAL
INSTANTIATE_TEST_CASE_P(SingleLayer, clsRasterDataSplitMerge,
                        Values(new InputRasterFiles(mask_asc_file, rs1_asc),
                            new InputRasterFiles(mask_tif_file, rs1_tif)));
#else
INSTANTIATE_TEST_CASE_P(SingleLayer, clsRasterDataSplitMerge,
                        Values(new InputRasterFiles(mask_asc_file, rs1_asc)));
#endif /* USE_GDAL */

} /* namespace */
