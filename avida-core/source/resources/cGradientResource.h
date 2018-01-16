//
//  cGradientResource.hpp
//  avida-core (static)
//
//  Created by Matthew Rupp on 1/15/18.
//

#ifndef cGradientResource_h
#define cGradientResource_h

#include "cAbstractResource.h"
#include "cAbstractResourceAcct.h"

class cGradientResourceAcct;

class cGradientResource : public cAbstractResource
{
  friend cGradientResourceAcct;
  
  private:
    cGradientResource();
    cGradientResource(const cGradientResource&);
    cGradientResource& operator=(const cGradientResource&);

  protected:
    int m_peaks; //JW
    double m_min_height; //JW
    double m_height_range; //JW
    double m_min_radius; //JW
    double m_radius_range; //JW
    double m_ah; //JW
    double m_ar; //JW
    double m_acx; //JW
    double m_acy; //JW
    double m_hstepscale; //JW
    double m_rstepscale; //JW
    double m_cstepscalex; //JW
    double m_cstepscaley; //JW
    double m_hstep; //JW
    double m_rstep; //JW
    double m_cstepx; //JW
    double m_cstepy; //JW
    int m_update_dynamic; //JW
    int m_peakx;
    int m_peaky;
    int m_height;
    int m_spread;    
    double m_plateau;
    int m_decay;
    int m_max_x;
    int m_max_y;
    int m_min_x;
    int m_min_y;
    double m_move_a_scalar;
    int m_updatestep; 
    int m_halo;
    int m_halo_inner_radius;
    int m_halo_width;
    int m_halo_anchor_x;
    int m_halo_anchor_y;
    int m_move_speed;
    int m_move_resistance;
    double m_plateau_inflow;
    double m_plateau_outflow;
    double m_cone_inflow;
    double m_cone_outflow;
    double m_gradient_inflow;
    int m_is_plateau_common;
    double m_floor;
    int m_habitat;
    int m_min_size;
    int m_max_size;
    int m_config;
    int m_count; 
    double m_resistance;
    double m_init_plat;
    double m_threshold;
    int m_refuge;
    Apto::Array<int> cell_id_list;
    double m_predator_odds;
    bool m_predator;
    double m_guard_juvs_per;

    
  public:
    cGradientResource(int id, const cString& name)
    : cAbstractResource(id, name) {}
    
    ADD_RESOURCE_PROP(int, Peaks, m_peaks);
    ADD_RESOURCE_PROP(double, MinHeight, m_min_height);
    ADD_RESOURCE_PROP(double, HeightRange, m_height_range);
    ADD_RESOURCE_PROP(double, MinRadius, m_min_radius);
    ADD_RESOURCE_PROP(double, RadiusRange, m_radius_range);
    ADD_RESOURCE_PROP(double, AH, m_ah);
    ADD_RESOURCE_PROP(double, AR, m_ar);
    ADD_RESOURCE_PROP(double, ACX, m_acx);
    ADD_RESOURCE_PROP(double, ACY, m_acy);
    ADD_RESOURCE_PROP(double, HStepScale, m_hstepscale);
    ADD_RESOURCE_PROP(double, RStepScale, m_rstepscale);
    ADD_RESOURCE_PROP(double, CStepScaleX, m_cstepscalex);
    ADD_RESOURCE_PROP(double, CStepScaleY, m_cstepscaley);
    ADD_RESOURCE_PROP(double, HStep, m_hstep);
    ADD_RESOURCE_PROP(double, RStep, m_rstep);
    ADD_RESOURCE_PROP(double, CStepX, m_cstepx);
    ADD_RESOURCE_PROP(double, CStepY, m_cstepy);
    ADD_RESOURCE_PROP(int, UpdateDynamic, m_update_dynamic);
    ADD_RESOURCE_PROP(int, PeakX, m_peakx);
    ADD_RESOURCE_PROP(int, PeakY, m_peaky);
    ADD_RESOURCE_PROP(int, Height, m_height);
    ADD_RESOURCE_PROP(int, Spread, m_spread);
    ADD_RESOURCE_PROP(double, Plateau, m_plateau);
    ADD_RESOURCE_PROP(int, Decay, m_decay);
    ADD_RESOURCE_PROP(int, MaxX, m_max_x);
    ADD_RESOURCE_PROP(int, MaxY, m_max_y);
    ADD_RESOURCE_PROP(int, MinX, m_min_x);
    ADD_RESOURCE_PROP(int, MinY, m_min_y);
    ADD_RESOURCE_PROP(double, MoveAScalar, m_move_a_scalar);
    ADD_RESOURCE_PROP(int, UpdateStep, m_updatestep);
    ADD_RESOURCE_PROP(int, Halo, m_halo);
    ADD_RESOURCE_PROP(int, HaloInnerRadius, m_halo_inner_radius);
    ADD_RESOURCE_PROP(int, HaloAnchorX, m_halo_anchor_x);
    ADD_RESOURCE_PROP(int, HaloAnchorY, m_halo_anchor_y);
    ADD_RESOURCE_PROP(int, MoveSpeed, m_move_speed);
    ADD_RESOURCE_PROP(int, MoveResistance, m_move_resistance);
    ADD_RESOURCE_PROP(double, PlateauInflow, m_plateau_inflow);
    ADD_RESOURCE_PROP(double, PlateauOutflow, m_plateau_outflow);
    ADD_RESOURCE_PROP(double, ConeInflow, m_cone_inflow);
    ADD_RESOURCE_PROP(double, ConeOutflow, m_cone_outflow);
    ADD_RESOURCE_PROP(double, GradientInflow, m_gradient_inflow);
    ADD_RESOURCE_PROP(int, IsPlateauCommon, m_is_plateau_common);
    ADD_RESOURCE_PROP(double, Floor, m_floor);
    ADD_RESOURCE_PROP(int, Habitat, m_habitat);
    ADD_RESOURCE_PROP(int, MinSize, m_min_size);
    ADD_RESOURCE_PROP(int, MaxSize, m_max_size);
    ADD_RESOURCE_PROP(int, Count, m_count);
    ADD_RESOURCE_PROP(double, Resistance, m_resistance);
    ADD_RESOURCE_PROP(double, InitialPlateau, m_init_plat);
    ADD_RESOURCE_PROP(double, Threshold, m_threshold);
    ADD_RESOURCE_PROP(double, PredatorOdds, m_predator_odds);
    ADD_RESOURCE_PROP(double, Damage, m_damage);
    ADD_RESOURCE_PROP(bool, IsPath, m_is_path);

};

class cGradientResourceAcct : public cAbstractResourceAcct
{
  
  
};
#endif /* cGradientResource_h */
