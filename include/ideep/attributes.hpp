#ifndef IDEEP_ATTRIBUTES_HPP
#define IDEEP_ATTRIBUTES_HPP

#include "abstract_types.hpp"
#include "utils.hpp"

namespace ideep {

using post_ops = dnnl::post_ops;

/// Attribute class for extra information into computations
struct attr_t : public dnnl::primitive_attr {
  attr_t() {}

  attr_t(const attr_t& other) : dnnl::primitive_attr(other) {
    *this = other;
  }

  attr_t(int mask, const scale_t& scales) {
    set_output_scales(mask, scales);
  }

  attr_t(dnnl_fpmath_mode_t fpmath_mode,
         dnnl::scratchpad_mode sp_mode = dnnl::scratchpad_mode::user) {
    set_fpmath_mode(fpmath_mode);
    set_scratchpad_mode(sp_mode);
  }

  attr_t& set_fpmath_mode(dnnl_fpmath_mode_t mode) {
    error::wrap_c_api(
        dnnl_primitive_attr_set_fpmath_mode(get(), mode),
        "could not set fpmath mode primitive attribute");
    return *this;
  }

  std::pair<scale_t, int> get_output_scales() const {
    dnnl_dim_t count;
    int c_mask;
    const float* c_scales;
    error::wrap_c_api(
        dnnl_primitive_attr_get_output_scales(
            get(), &count, &c_mask, &c_scales),
        "could not get int output scales");
    return std::make_pair(scale_t(c_scales, c_scales + count), c_mask);
  }

  std::pair<zero_point_t, int> get_zero_points(int arg) const {
    int mask;
    zero_point_t zero_points;
    get_zero_points(arg, mask, zero_points);
    return std::make_pair(zero_points, mask);
  }

  void get_zero_points(
          int arg, int &mask, std::vector<int32_t> &zero_points) const {
      dnnl::primitive_attr::get_zero_points(arg, mask, zero_points);
  }

  // Helper factory
  static attr_t fuse_sum(float scale = 1.0) {
    attr_t attr;
    post_ops po;
    po.append_sum(scale);
    attr.set_post_ops(po);
    return attr;
  }

  static attr_t fuse_relu(
      float scale = 1.0,
      float alpha = 0.f,
      float beta = 0.f) {
    attr_t attr;
    post_ops po;
    po.append_eltwise(scale, algorithm::eltwise_relu, alpha, beta);
    attr.set_post_ops(po);
    return attr;
  }

  static attr_t fuse_gelu(
      float scale = 1.0,
      float alpha = 0.f,
      float beta = 0.f,
      algorithm gelu_type = algorithm::eltwise_gelu_erf) {
    attr_t attr;
    post_ops po;
    po.append_eltwise(scale, gelu_type, alpha, beta);
    attr.set_post_ops(po);
    return attr;
  }

  static attr_t fuse_elu(
      float scale = 1.0,
      float alpha = 0.f,
      float beta = 1.0) {
    attr_t attr;
    post_ops po;
    po.append_eltwise(scale, algorithm::eltwise_elu, alpha, beta);
    attr.set_post_ops(po);
    return attr;
  }

  static attr_t fuse_sigmoid(
      float scale = 1.0,
      float alpha = 1.0,
      float beta = 0.f) {
    attr_t attr;
    post_ops po;
    po.append_eltwise(scale, algorithm::eltwise_logistic, alpha, beta);
    attr.set_post_ops(po);
    return attr;
  }

  static attr_t fuse_swish(
      float scale = 1.0,
      float alpha = 1.0,
      float beta = 0.f) {
    attr_t attr;
    post_ops po;
    po.append_eltwise(scale, algorithm::eltwise_swish, alpha, beta);
    attr.set_post_ops(po);
    return attr;
  }

  static attr_t fuse_tanh(
      float scale = 1.0,
      float alpha = 0.f,
      float beta = 0.f) {
    attr_t attr;
    post_ops po;
    po.append_eltwise(scale, algorithm::eltwise_tanh, alpha, beta);
    attr.set_post_ops(po);
    return attr;
  }

  static attr_t fuse_mish(
      float scale = 1.0,
      float alpha = 1.0,
      float beta = 0.f) {
    attr_t attr;
    post_ops po;
    po.append_eltwise(scale, algorithm::eltwise_mish, alpha, beta);
    attr.set_post_ops(po);
    return attr;
  }

  static attr_t residual(
      float sum_scale = 1.0,
      float relu_scale = 1.0,
      float alpha = 0.f,
      float beta = 0.f) {
    attr_t attr;
    post_ops po;
    po.append_sum(sum_scale);
    po.append_eltwise(relu_scale, algorithm::eltwise_relu, alpha, beta);
    attr.set_post_ops(po);
    return attr;
  }

  static attr_t fuse_clamp(float lower_bound = -1.0, float upper_bound = 1.0) {
    attr_t attr;
    post_ops po;
    po.append_eltwise(1.0, algorithm::eltwise_clip, lower_bound, upper_bound);
    attr.set_post_ops(po);
    return attr;
  }

  static attr_t fuse_hardswish(
      float scale = 1.0,
      float alpha = 1.0,
      float beta = 0.f) {
    attr_t attr;
    post_ops po;
    po.append_eltwise(scale, algorithm::eltwise_hardswish, alpha, beta);
    attr.set_post_ops(po);
    return attr;
  }

  static attr_t fuse_abs(
      float scale = 1.0,
      float alpha = 1.0,
      float beta = 0.f) {
    attr_t attr;
    post_ops po;
    po.append_eltwise(scale, algorithm::eltwise_abs, alpha, beta);
    attr.set_post_ops(po);
    return attr;
  }

  static attr_t fuse_exp(
      float scale = 1.0,
      float alpha = 1.0,
      float beta = 0.f) {
    attr_t attr;
    post_ops po;
    po.append_eltwise(scale, algorithm::eltwise_exp, alpha, beta);
    attr.set_post_ops(po);
    return attr;
  }

  static attr_t fuse_square(
      float scale = 1.0,
      float alpha = 1.0,
      float beta = 0.f) {
    attr_t attr;
    post_ops po;
    po.append_eltwise(scale, algorithm::eltwise_square, alpha, beta);
    attr.set_post_ops(po);
    return attr;
  }

  static attr_t fuse_log(
      float scale = 1.0,
      float alpha = 1.0,
      float beta = 0.f) {
    attr_t attr;
    post_ops po;
    po.append_eltwise(scale, algorithm::eltwise_log, alpha, beta);
    attr.set_post_ops(po);
    return attr;
  }

  static attr_t fuse_round(
      float scale = 1.0,
      float alpha = 1.0,
      float beta = 0.f) {
    attr_t attr;
    post_ops po;
    po.append_eltwise(scale, algorithm::eltwise_round, alpha, beta);
    attr.set_post_ops(po);
    return attr;
  }

  static attr_t fuse_sqrt(
      float scale = 1.0,
      float alpha = 1.0,
      float beta = 0.f) {
    attr_t attr;
    post_ops po;
    po.append_eltwise(scale, algorithm::eltwise_sqrt, alpha, beta);
    attr.set_post_ops(po);
    return attr;
  }

  static attr_t fuse_pow(
      float scale = 1.0,
      float alpha = 1.0,
      float beta = 0.f) {
    attr_t attr;
    post_ops po;
    po.append_eltwise(scale, algorithm::eltwise_pow, alpha, beta);
    attr.set_post_ops(po);
    return attr;
  }

  static attr_t fuse_hardsigmoid() {
    constexpr float scale = 1.0f;
    constexpr float alpha = 1.0f / 6.0f;
    constexpr float beta = 1.0f / 2.0f;

    attr_t attr;
    post_ops po;
    po.append_eltwise(scale, algorithm::eltwise_hardsigmoid, alpha, beta);
    attr.set_post_ops(po);
    return attr;
  }

  static attr_t fuse_binary(algorithm alg, memory::desc src_desc) {
    attr_t attr;
    post_ops po;
    po.append_binary(alg, src_desc);
    attr.set_post_ops(po);
    return attr;
  }

  static attr_t attr_post_ops(post_ops po) {
    attr_t attr;
    attr.set_post_ops(po);
    return attr;
  }

  bool has_op_kind(kind op_kind) const {
    auto po = get_post_ops();
    for (int i = 0; i < po.len(); i++)
      if (op_kind == po.kind(i))
        return true;
    return false;
  }

  bool has_post_op() const {
    auto po = get_post_ops();
    return po.len() > 0;
  }

  std::tuple<kind, float, float, float, algorithm> get_params(int index) const {
    auto po = get_post_ops();
    IDEEP_ENFORCE(index < po.len(), "post_ops index is out of range");

    algorithm alg = algorithm::undef;
    float scale = 1.0, alpha = 1.0, beta = 0.0;
    memory::desc binary_src_desc;

    auto akind = po.kind(index);
    switch (akind) {
      case kind::sum:
        po.get_params_sum(index, scale);
        break;
      case kind::eltwise:
        po.get_params_eltwise(index, scale, alg, alpha, beta);
        break;
      case kind::binary:
        po.get_params_binary(index, alg, binary_src_desc);
        break;
      default:
        error::wrap_c_api(dnnl_invalid_arguments, "could not get params");
        break;
    }

    return std::make_tuple(akind, scale, alpha, beta, alg);
  }

  bool non_negitive_output() const {
    auto po = get_post_ops();
    auto last = po.len() - 1;
    if (last < 0) {
      return false;
    }

    auto params = get_params(last);
    if (std::get<0>(params) != kind::eltwise || std::get<1>(params) <= 0.f ||
        std::get<2>(params) != 0.f || std::get<3>(params) != 0.f ||
        std::get<4>(params) != algorithm::eltwise_relu)
      return false;

    return true;
  }

  attr_t& operator=(const attr_t& rhs) {
    if (this == &rhs) {
      return *this;
    }
    dnnl_primitive_attr_t result;
    error::wrap_c_api(
        dnnl_primitive_attr_clone(&result, rhs.get()),
        "could not clone primitive attributes");
    this->reset(result);
    return *this;
  }

  bool operator==(const attr_t& rhs) const {
    auto l_po = get_post_ops();
    auto r_po = rhs.get_post_ops();
    if (l_po.len() != r_po.len() ||
        get_output_scales() != rhs.get_output_scales() ||
        get_fpmath_mode() != rhs.get_fpmath_mode() ||
        get_scratchpad_mode() != rhs.get_scratchpad_mode()) {
      return false;
    }
    if (get_zero_points(DNNL_ARG_SRC) != rhs.get_zero_points(DNNL_ARG_SRC) ||
        get_zero_points(DNNL_ARG_WEIGHTS) != rhs.get_zero_points(DNNL_ARG_WEIGHTS) ||
        get_zero_points(DNNL_ARG_DST) != rhs.get_zero_points(DNNL_ARG_DST)) {
      return false;
    }
    for (auto index = 0; index < l_po.len(); index++) {
      kind l_akind, r_akind;
      algorithm l_alg, r_alg;
      float l_scale = 1.0, l_alpha = 1.0, l_beta = 0.0;
      float r_scale = 1.0, r_alpha = 1.0, r_beta = 0.0;
      std::tie(l_akind, l_scale, l_alpha, l_beta, l_alg) = get_params(index);
      std::tie(r_akind, r_scale, r_alpha, r_beta, r_alg) =
          rhs.get_params(index);
      if (l_akind != r_akind || l_alg != r_alg || l_scale != r_scale ||
          l_alpha != r_alpha || l_beta != r_beta) {
        return false;
      }
    }
    return true;
  }

  void to_bytes(utils::bytestring& bytes) const {
    // encode post ops
    auto num_ops = get_post_ops().len();
    for (int i = 0; i < num_ops; i++) {
      kind akind;
      algorithm alg = algorithm::undef;
      float scale = 1.0, alpha = 1.0, beta = 0.0;
      std::tie(akind, scale, alpha, beta, alg) = get_params(i);

      switch (akind) {
        case kind::sum:
          utils::to_bytes(bytes, akind);
          bytes.append(1, '.');
          utils::to_bytes(bytes, scale);
          break;
        case kind::eltwise:
          utils::to_bytes(bytes, akind);
          bytes.append(1, '.');
          utils::to_bytes(bytes, scale);
          bytes.append(1, '.');
          utils::to_bytes(bytes, alpha);
          bytes.append(1, '.');
          utils::to_bytes(bytes, beta);
          bytes.append(1, '.');
          utils::to_bytes(bytes, alg);
        case kind::binary:
          utils::to_bytes(bytes, akind);
          bytes.append(1, '.');
          utils::to_bytes(bytes, alg);
        default:
          break;
      }
    }

    // encode output scales
    auto scales = get_output_scales();
    utils::to_bytes(bytes, scales.first);
    utils::to_bytes(bytes, scales.second);

    // Note: depthwise/binary post op, zero points, scales, rnn params are
    // not encoded so far. PD cache is supposed to use in convolution only
    // as a temporary workaround for gemm-based conv pd overhead
  }
};

} // namespace ideep

#endif