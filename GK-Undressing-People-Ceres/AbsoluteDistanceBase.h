#pragma once
#include <ceres/ceres.h>
#include <igl/point_mesh_squared_distance.h>
#include <igl/signed_distance.h>

#include <GeneralMesh/GeneralMesh.h>
#include "SMPLWrapper.h"

class AbsoluteDistanceBase : public ceres::CostFunction
{
public:
    AbsoluteDistanceBase(SMPLWrapper*, GeneralMesh *, double pruning_threshold = 100.);
    ~AbsoluteDistanceBase();

protected:

    // 
    template<typename Row1, typename Row2>
    inline double residual_elem_(const double signed_dist, 
        const Row1 vertex_normal, const Row2 input_normal) const
    {
        // TODO remove the check for the translation and pose optimization cases
        if (abs(signed_dist) > pruning_threshold_)  // prune far away coorespondences
            return 0;
        if (vertex_normal.dot(input_normal) <= 0)   // do not account for a point, if the normals don't match
            return 0;
        
        return abs(signed_dist);
    }

    //
    template<typename Row1, typename Row2, typename Row3>
    inline double pose_jac_elem_(const Row1&& vertex,
        const Row2&& closest_input_point,
        double abs_dist,
        const Row3&& grad) const
    {
        double jac_entry = abs_dist < 1e-5
            ? 0
            : (vertex - closest_input_point).dot(grad) / abs_dist;

        return jac_entry;
    }

    //
    template<typename Row1, typename Row2, typename Row3>
    inline double shape_jac_elem_(const Row1&& vertex,
        const Row2&& closest_input_point,
        double abs_dist,
        const Row3&& grad) const
    {
        double jac_entry = abs_dist < 1e-5
            ? 0
            : (vertex - closest_input_point).dot(grad) / abs_dist;

        return jac_entry;
    }

    //
    inline double translation_jac_elem_(const double vert_coord,
        const double input_coord, double abs_dist) const
    {
        double jac_entry = abs_dist < 1e-5
            ? 0
            : (vert_coord - input_coord) / abs_dist;
        return jac_entry;
    }


    GeneralMesh * toMesh_;
    SMPLWrapper * smpl_;
    double pruning_threshold_;
};

