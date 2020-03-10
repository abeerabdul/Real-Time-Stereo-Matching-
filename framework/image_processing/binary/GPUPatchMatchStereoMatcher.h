#ifndef DSM_GPU_PATCH_MATCH_STEREO_MATCHER_H
#define DSM_GPU_PATCH_MATCH_STEREO_MATCHER_H

#include <image_processing/base/GPUImageProcessorBinary.h>

#include <image_processing/unary/GPUImageConverter.h>
#include <map>

namespace dsm {


class GPUPatchMatchStereoMatcher : public GPUImageProcessorBinary {
public:


	//factory function
	static std::shared_ptr<GPUPatchMatchStereoMatcher> create(cl_context const& context, cl_device_id const& device_id,
												              cv::Vec2i const& image_dimensions);
	
	GPUPatchMatchStereoMatcher(cl_context const& context, cl_device_id const& device_id, cv::Vec2i const& image_dims);
	~GPUPatchMatchStereoMatcher() {};

	virtual void process(cl_command_queue const& command_queue, 
						 cl_mem const& in_image_buffer_1, cl_mem const& in_image_buffer_2, cl_mem const& in_image_buffer_3, cl_mem const& in_image_buffer_4, 
						 cl_mem& out_image_buffer) override;

	void set_minimum_disparity(cl_context const& context, cl_device_id const& device_id, float in_minimum_disparity);
	void set_maximum_disparity(cl_context const& context, cl_device_id const& device_id, float in_maximum_disparity);
	void set_search_window_half_size(int in_search_window_half_size);
	void set_num_iterations(int in_num_iterations);
	void set_temp_propagation(int in_temp_propagation);
	void set_outlier_switch(int in_outlier_switch);
	void set_view_propagation(int in_switch_view_prop);
	void set_plane_refine_steps(int plane_refine_steps);
	void set_slanted_or_fronto(int slanted);
	//void set_compute_right(int value);
	std::string _random_string();

private:
    void reload_resources(cl_context const& context, cl_device_id const& device_id);
	virtual void _init_kernels(cl_context const& context, cl_device_id const& device_id) override;
	virtual void _init_memory_objects(cl_context const& context, cl_device_id const& device_id) override;

	virtual void _cleanup_kernels() override;
	virtual void _cleanup_memory_objects() override;

// non-overriding private member functions
private:
    void _register_kernel(cl_context const& context, cl_device_id const& device_id,
		     			  std::string const& kernel_path, std::string const& kernel_function_name,
						  cl_kernel& in_out_compiled_kernel, std::string kernel_defines = "");
	
	// initialize converter and other auxiliary processors needed for this class
    void _init_helper_processors(cl_context const& context, cl_device_id const& device_id, cv::Vec2i const& image_dims);


	void _init_random_initialization_kernel(cl_context const& context, cl_device_id const& device_id);
	void _init_spatial_propagation_kernel(cl_context const& context, cl_device_id const& device_id);
	void _init_view_propagation_kernel(cl_context const& context, cl_device_id const& device_id);
	void _init_plane_refinement_kernel(cl_context const& context, cl_device_id const& device_id);
	void _init_gradient_filter_kernel(cl_context const& context, cl_device_id const& device_id);
	void _init_rgb_and_gradient_combiner_kernel(cl_context const& context, cl_device_id const& device_id);

	void _init_disp_buffer_to_plane_image(cl_context const& context, cl_device_id const& device_id);

	void _init_copy_plane_to_last_plane(cl_context const& context, cl_device_id const& device_id);
	
	void _init_temporal_propagation_kernel(cl_context const& context, cl_device_id const& device_id);

	void _init_compute_limits_kernel(cl_context const& context, cl_device_id const& device_id);

	void _init_convert_RGBA_to_R_kernel(cl_context const& context, cl_device_id const& device_id);

	void _init_copy_grayscale_3x8_buffer_to_image_2D_kernel(cl_context const& context, cl_device_id const& device_id);

	void _init_copy_bgr_3x8_buffer_to_image_kernel(cl_context const& context, cl_device_id const& device_id);

	void _init_copy_image_2D_to_buffer_1x8_buffer_kernel(cl_context const& context, cl_device_id const& device_id);
	
	void _init_optimize_outlier_detection_kernel(cl_context const& context, cl_device_id const& device_id);

	void _init_optimize_fill_invalid_pixel_kernel(cl_context const& context, cl_device_id const& device_id);

	void _init_median_filter_float_disparity_3x3_kernel(cl_context const& context, cl_device_id const& device_id);

    void _set_arg_and_run_copy_3_channel_buffer_to_image_gray(cl_command_queue const& command_queue, cl_mem const &buffer, cl_mem const &input_image);

	void _set_arg_and_run_bgr_3x8_buffer_image_copy(cl_command_queue const& cl_command_queue, cl_mem const &buffer, cl_mem const &input_image);
	
    void _set_arg_and_run_copy_image_to_1_channel_buffer_gray(cl_command_queue const& command_queue, cl_mem const &input_image, 
															  cl_mem const &buffer);															  

    void _set_arg_and_run_compute_limits(cl_command_queue const& command_queue, 
                                    cl_mem disparity_image, cl_mem limits);

    void _set_arg_and_run_random_initialization(cl_command_queue const& command_queue, cl_mem& plane_image,
												float max_disparity, float min_disparity);

    void _set_arg_and_run_median_filter_float_disparity_3x3(cl_command_queue const& command_queue, 
    														cl_mem const& in_disparity_image, cl_mem const& in_color_image,cl_mem const& outlier_mask, cl_mem& out_disparity_image);

	void _set_arg_and_run_gradient_filter(cl_command_queue const& cl_command_queue,
										  cl_mem& input_image_left, cl_mem& input_image_right,
										  cl_mem& gradient_image_left, cl_mem& gradient_image_right);														

	void _set_arg_and_run_combine_rgb_and_gradient(cl_command_queue const& cl_command_queue,
										  		   cl_mem& in_rgb_left, cl_mem& in_rgb_right,
										  		   cl_mem& in_gradient_image_left, cl_mem& in_gradient_image_right,
										  		   cl_mem& out_rgb_grad_left, cl_mem& out_rgb_grad_right);
	void set_arg_and_run_disp_buffer_to_plane_image(cl_command_queue const& cl_command_queue, 
                                            		cl_mem const &in_image_buffer, cl_mem& in_out_disp_image, cl_mem& out_plane_image_3, cl_mem& out_plane_image_4,
													int min_disp, int max_disp); 	

	void _set_arg_and_run_copy_plane_image_to_last_plane(cl_command_queue const& command_queue,
														cl_mem &final_plane_left, cl_mem &final_plane_right,
														cl_mem &last_plane_left, cl_mem &last_plane_right);

    void _set_arg_and_run_propagation_iteration(cl_command_queue const& command_queue, 
                                                cl_mem& plane_image_left_a, cl_mem& plane_image_left_b,
                                                cl_mem& plane_image_right_a, cl_mem& plane_image_right_b,
									  			cl_mem const &input_image_left, cl_mem const &input_image_right,
												cl_mem &cl_plane_image_last_left, cl_mem& cl_plane_image_last_right,
												cl_mem& cl_gradient_image_left, cl_mem& cl_gradient_image_right,
												int num_iterations,
												float max_disparity,
												int radius,
												int compute_right, int switch_temp_prop, int switch_view_prop, float plane_refine_steps, int slanted_or_fronto);

    void _set_arg_and_run_convet_RGBA_to_R(cl_command_queue const& command_queue,  
                                           cl_mem &plane_image, cl_mem const &disparity_image, 
										   int num_iterations, float max_disparity);
										   
    void _set_arg_and_run_outlier_detection(cl_command_queue const& command_queue, 
											cl_mem const &disparity_image_left , cl_mem const &disparity_image_right, 
											cl_mem& disparity_image_out, 
											cl_mem& cl_disparity_outlier_mask,
											float min_disparity, float max_disparity);
    void _set_arg_and_run_outlier_detection_for_Convert_RGBA_in_R(cl_command_queue const& command_queue, 
											cl_mem const &disparity_image_left , cl_mem const &disparity_image_right, 
											cl_mem& disparity_image_out, 
											cl_mem& cl_disparity_outlier_mask,
											float min_disparity, float max_disparity);

	void _write_PNG_image(cl_command_queue const& command_queue, cl_mem &buffer);											

    std::string _kernel_defines_simple_patch_match();
// member variables
private:
	cl_kernel kernel_copy_grayscale_3x8_buffer_to_image_2D_ = 0;
	cl_kernel kernel_copy_image_2D_to_1x8_buffer_ = 0;

	cl_kernel kernel_copy_bgr_3x8_buffer_to_image_ = 0;

	cl_kernel kernel_temporal_propagation_ = 0;

	cl_kernel kernel_init_random_initialization_ = 0;
	cl_kernel kernel_init_spatial_propagation_ = 0;
	cl_kernel kernel_init_view_propagation_ = 0;
	cl_kernel kernel_init_plane_refinement_ = 0;
	cl_kernel kernel_init_view_propagation = 0;
	cl_kernel kernel_init_gradient_filter = 0;
	cl_kernel kernel_init_rgb_and_gradient_combiner = 0;
	cl_kernel kernel_init_copy_plane_image_to_last_plane = 0;
	cl_kernel kernel_compute_limits_ = 0;
	cl_kernel kernel_init_convert_RGBA_to_R_ = 0;

	cl_kernel kernel_copy_1x8_buffer_to_unnormalized_disp_img = 0;
	cl_kernel kernel_convert_disp_to_plane_img = 0;

	cl_kernel kernel_optimize_outlier_detection_ = 0;
	cl_kernel kernel_optimize_fill_invalid_pixel_ = 0;
	cl_kernel kernel_median_filter_float_disparity_3x3_ = 0;

	int search_window_half_size_ = 11;
	float minimum_disparity_ = 0;
	float maximum_disparity_ = 60;
	int num_iterations_ = 4;
	int temp_prop_ = 0; 
	int view_prop_ = 0;
	int outlier_switch = 0;
	float plane_refine_steps_ = 2;
	int slanted_or_fronto_ = 0;
	//int compute_right_ = 0;

//memory buffers
private:  
    cl_mem cl_left_input_image_ = 0;
    cl_mem cl_right_input_image_ = 0;

	cl_mem cl_disp_initial_guess_image_3_ = 0;
	cl_mem cl_disp_initial_guess_image_4_ = 0;
	cl_mem cl_plane_initial_guess_image_ = 0;
	
	cl_mem cl_left_input_rgb_ = 0;
	cl_mem cl_right_input_rgb_ = 0;

	cl_mem cl_left_input_rgb_grad_ = 0;
	cl_mem cl_right_input_rgb_grad_ = 0;

    cl_mem cl_plane_image_left_a_ = 0;
    cl_mem cl_plane_image_left_b_ = 0;
    cl_mem cl_plane_image_right_a_ = 0;
    cl_mem cl_plane_image_right_b_ = 0;

    cl_mem cl_disparity_image_left_ = 0;
    cl_mem cl_disparity_image_left_filtered_ = 0;
    cl_mem cl_disparity_image_right_ = 0;

	cl_mem cl_gradient_image_left_ = 0;
	cl_mem cl_gradient_image_right_ = 0;

	cl_mem cl_plane_image_last_left = 0;
	cl_mem cl_plane_image_last_right = 0;

	cl_mem cl_disparity_image_outlier_detection_ = 0;
	cl_mem cl_disparity_outlier_mask_1_ = 0;
	cl_mem cl_disparity_outlier_mask_2_ = 0;
	cl_mem cl_limits_left_ = 0;
	cl_mem cl_limits_right_ = 0;
	
};


} ///namespace dsm


#endif // DSM_GPU_PATCH_MATCH_STEREO_MATCHER_H