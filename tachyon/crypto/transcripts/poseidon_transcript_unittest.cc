// Copyright 2020-2022 The Electric Coin Company
// Copyright 2022 The Halo2 developers
// Use of this source code is governed by a MIT/Apache-2.0 style license that
// can be found in the LICENSE-MIT.halo2 and the LICENCE-APACHE.halo2
// file.

#include "tachyon/crypto/transcripts/poseidon_transcript.h"

#include <utility>
#include <vector>

#include "gtest/gtest.h"

#include "tachyon/math/elliptic_curves/bn/bn254/g1.h"

namespace tachyon::crypto {

namespace {

class PoseidonTranscriptTest : public testing::Test {
 public:
  static void SetUpTestSuite() { math::bn254::G1Curve::Init(); }
};

}  // namespace

TEST_F(PoseidonTranscriptTest, WritePoint) {
  using Curve = math::bn254::G1Curve;

  base::VectorBuffer write_buf;
  PoseidonWriter<Curve> writer(std::move(write_buf));
  Curve::AffinePointTy expected = Curve::AffinePointTy::Random();
  ASSERT_TRUE(writer.WriteToProof(expected));

  base::Buffer read_buf(writer.buffer().buffer(), writer.buffer().buffer_len());
  PoseidonReader<Curve> reader(std::move(read_buf));
  Curve::AffinePointTy actual;
  ASSERT_TRUE(reader.ReadPoint(&actual));

  EXPECT_EQ(expected, actual);
}

TEST_F(PoseidonTranscriptTest, WriteScalar) {
  using Curve = math::bn254::G1Curve;

  base::VectorBuffer write_buf;
  PoseidonWriter<Curve> writer(std::move(write_buf));
  Curve::ScalarField expected = Curve::ScalarField::Random();
  ASSERT_TRUE(writer.WriteToProof(expected));

  base::Buffer read_buf(writer.buffer().buffer(), writer.buffer().buffer_len());
  PoseidonReader<Curve> reader(std::move(read_buf));
  Curve::ScalarField actual;
  ASSERT_TRUE(reader.ReadScalar(&actual));

  EXPECT_EQ(expected, actual);
}

TEST_F(PoseidonTranscriptTest, SqueezeChallenge) {
  using Curve = math::bn254::G1Curve;

  base::VectorBuffer write_buf;
  PoseidonWriter<Curve> writer(std::move(write_buf));
  Curve::AffinePointTy generator = Curve::AffinePointTy::Generator();
  ASSERT_TRUE(writer.WriteToProof(generator));

  std::vector<uint8_t> expected_bytes = {25,  86,  205, 219, 59,  135, 187, 231,
                                         192, 54,  23,  138, 114, 176, 9,   157,
                                         1,   97,  110, 174, 67,  9,   89,  85,
                                         126, 129, 216, 121, 53,  99,  227, 26};
  Curve::ScalarField expected = Curve::ScalarField::FromBigInt(
      math::BigInt<4>::FromBytesLE(expected_bytes));

  Curve::ScalarField actual = writer.SqueezeChallenge().ChallengeAsScalar();

  EXPECT_EQ(expected, actual);
}

}  // namespace tachyon::crypto