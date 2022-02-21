//CUSTOMIZER VARIABLES

/* [Basic_Parameters] */

// Stepper Motor Size
motor_index = 4;	// [0:NEMA 8, 1:NEMA 11, 2:NEMA 14, 3:NEMA 16, 4:NEMA 17, 5:NEMA 23, 6:NEMA 34, 7:NEMA 42]

// Parallel or Perpendicular (will need supports)
orientation = 0;	// [0:Parallel, 1:Perpendicular]

// mount thickness, in mm
thickness = 2.0;    // [2:0.1:10]

// Openbeam size, in mm
openbeam_size = 10;   // [10:50]

// Openbeam bolt diameter, in mm
beam_bolt_diameter = 3.0;    // [2:0.1:40]

// Openbeam slot diameter for tight fit, in mm
slot_diameter = 3.1;    // [2:0.1:40]

// Openbeam slot distance to edge for tight fit, in mm
slot_depth = 4.5;    // [2:0.1:40]

//CUSTOMIZER VARIABLES END



// use motor size choice to set the important values

// Values are from multiple sources - nobody seems to have all the numbers right in one place!
// And even different manufacturers can't quite agree on all the dimensions!
// I'm open to corrections and additions - but there had better be a good source behind the new values.

// [ motor width in mm (calculated from inches/10), bolt size in mm, bolt spacing in mm, center hole diameter in mm, name ]
motor_templates = [
    [ 0.8*25.4,   2,    16,   15, "NEMA 8"  ],
    [ 1.1*25.4, 2.5,    23,   22, "NEMA 11" ],
    [ 1.4*25.4,   3,    26,   22, "NEMA 14" ],
    [ 1.6*25.4,   3,    31,   22, "NEMA 16" ],
    [ 1.7*25.4,   3,    31,   22, "NEMA 17" ],
    [ 2.3*25.4, 5.0,  47.1, 38.1, "NEMA 23" ],
    [ 3.4*25.4, 6.5,  69.6,   73, "NEMA 34" ],
    [ 4.2*25.4, 8.5,  89.0, 55.5, "NEMA 42" ],
];

motor_width = motor_templates[motor_index][0];
bolt_diameter = motor_templates[motor_index][1];
bolt_radius = (bolt_diameter+1) / 2;
bolt_spacing = motor_templates[motor_index][2];
bolt_inset = (motor_width - bolt_spacing) / 2;
center_diameter = motor_templates[motor_index][3];
center_radius = (center_diameter+4) / 2; // 3 mm margin wasn't quite enough on some motors

temp_length = max( motor_width, 5*beam_bolt_diameter );
temp_width = max( motor_width, openbeam_size+thickness*2 );
temp_length2 = max( motor_width, openbeam_size+thickness*2 );
temp_width2 = max( motor_width, 5*beam_bolt_diameter );
mount_width = (orientation == 1) ? temp_width2 : temp_width;
mount_length = (orientation == 1) ? temp_length2 : temp_length  + 25;
mount_offset_x = (mount_width - motor_width) / 2;
mount_offset_z = (mount_length - motor_width) / 2;
beam_bolt_radius = (beam_bolt_diameter+1) / 2;


// Sanity checks
if( motor_width <= 0 || bolt_diameter <= 0
    || openbeam_size <= 0 || beam_bolt_diameter <= 0
    || slot_diameter <= 0 || slot_depth <= 0)
{
    echo("<B>Error: Missing important parameters, possibly wrong motor index</B>");
}

// Sanity checks
if( openbeam_size <= 2*beam_bolt_diameter ||
    openbeam_size <= 2*slot_diameter ||
    openbeam_size <= 2*slot_depth)
{
    echo("<B>Error: Parameters result in bad beam attachment</B>");
}

//use <T_Slot.scad>
//use <T-slot_nut.scad>


// Vertical mounting
//rotate([-90,0,90])
    //TSlotNut(5);

//translate([-3,0,19])
    //2020Profile(30);
// our object, flat on xy plane for easy STL generation

//rotate([0,0,90])
//prism(10, 5, 5);

//translate([-5,0,5])
//rotate([90,0,90])
//prism(10, 5, 5);


// motor mount

/*
mirror([0,1,0])
translate([2,0,2])
rotate([90,0,0])
motor_mount();
*/

//motor_mount();

//rotate([90,0,0])
servo_mount();

// tslot beam
//tslot_beam();

//t_slot_beam(35,10);

//mirror([0,1,0])
//base(15, 35+9.5);
//translate([10,-3,34])
//rotate([90,0,0])
    //TSlotNut(5);


//translate([(mount_width-mount_offset_x)/2/2/2/2/2/2/2/2,-3,temp_length+2])
//rotate([-90,0,0])
    //beam_mount();

// beam base
//beam_base();

//translate([mount_width/2-mount_offset_x,25,37])
//rotate([90,0,0])
//linear_extrude(height=30)
    //import("openbeam.dxf");

module prism(l, w, h){
   polyhedron(
           points=[[0,0,0], [l,0,0], [l,w,0], [0,w,0], [0,w,h], [l,w,h]],
           faces=[[0,1,2,3],[5,4,3,2],[0,4,5,1],[0,3,4],[5,2,1]]
           );
   }


module base(t_thickness, slot_height=35) {
    
    thickness = 4;
    m_width = 39+0.2;
    m_height = 15+0.3;  // 10 for DMP140
    //m_height = 10+1;
    padded_width = m_width + thickness*2;
    padded_height = m_height + thickness*2;
    
    // prism length
    p_length = 2.5;  // 4.2 for DMP140
    //p_length = 4.0;
    p_width = 2.0;
    //p_width = 4.0;
    
    
    difference () {
        
        // stuff to build up
        union() { 
            
            // original cube
            cube([padded_width, t_thickness, padded_height]);
            
            
        }
        
        // things to cut out
        
        translate([padded_width/2-((m_width)/2), 0,padded_height/2-(m_height/2)])
        cube([m_width, t_thickness, m_height]);
        
        // screw
        translate([m_width-(3.8/2)+1.9+0.8,t_thickness/2-3,0])
        cylinder(thickness, d=3.8);
        
    }
    
    //gap = 1.5;
    gap = 2.5;
    
    union () {
        
      // triangles supports
      
      translate([thickness+m_width,t_thickness,thickness+gap])
        rotate([90,0,-90])
        prism(t_thickness, p_length, p_width);  
      
      translate([thickness+m_width,0,padded_height-thickness-gap])  
      rotate([-90,0,90])
        prism(t_thickness, p_length, p_width);
      
      translate([thickness,0,thickness+gap])  
      rotate([90,0,90])
       prism(t_thickness,p_length, p_width); 
        
      translate([thickness,t_thickness,padded_height-thickness-gap])  
      rotate([-90,0,-90])
        prism(t_thickness, p_length, p_width);
        
        
      // extended beam
      slot_thickness = 2;
      t_padded_width = motor_width + slot_thickness*2;
      extra_length = 22+10;
      t_slot_thickness = 10;
      
      translate([padded_width,0,0])
      cube([extra_length-1, t_thickness, padded_height]);
      
      translate([padded_width+extra_length-1,t_thickness-t_slot_thickness,0])
      cube([1, t_slot_thickness, padded_height]);
      
      translate([padded_width+extra_length,t_thickness-t_slot_thickness,0])
      cube([t_padded_width, t_slot_thickness, padded_height/2]);
      
      
      translate([extra_length+padded_width,t_thickness-t_slot_thickness,padded_height/2])
      t_slot_beam(slot_height, t_slot_thickness);
        
    }
    
   
}


module t_slot_beam(t_length, t_thickness) {
    slot_length = t_length;
    slot_thickness = t_thickness;
    padded_width = mount_width+thickness*2;
    
    difference() {
        
        // stuff to build up
        union() {
            
            
            cube([padded_width, slot_thickness, slot_length]);
        }
        
        //things to cut out
        t_slot_length = 3;
         t_slot_width = 15;
         t_slot_thickness = 6;
        
        translate([padded_width/2-(t_slot_thickness/2),0, 0])
        cube([t_slot_thickness, t_slot_length, slot_length]);
        
        translate([padded_width/2-(t_slot_width/2),t_slot_length, 0])
        cube([t_slot_width, thickness+3, slot_length]);
        
        translate([padded_width/2-(4/2),t_slot_length+thickness+3, slot_length/2-padded_width/2-5])
        cube([4,2,slot_length-10.5]);
    }
    

}
module servo_mount() {
    thickness = 2.0;
    servo_length = 40 + 0.5;
    servo_width = 20 + 0.5;
    servo_height = 27.8;
    protuding_length = 7.25+0.5;
    
    t_slot_length = 3+0.3;
    t_slot_width = 15-0.4+20;
    t_slot_thickness = 6-0.4+20;
    
    padded_length = servo_length + thickness*2;
    padded_width = servo_width + thickness*2;
    
    padded_motor_width = mount_width+thickness*2;
    
    difference() {
        union() {
            translate([0,-(padded_motor_width/4),0])
            cube([padded_length,padded_motor_width,servo_height]);
            
            translate([servo_length+thickness,-(padded_motor_width/4),0])
            cube([protuding_length,padded_motor_width,servo_height]);
            translate([-protuding_length+thickness,-(padded_motor_width/4),0])
            cube([protuding_length,padded_motor_width,servo_height]);
            
            //t slot
            xtra = 11.5;

            translate([-xtra/2,(padded_width-t_slot_thickness)/2,-(t_slot_length)])
            cube([padded_length+xtra,t_slot_thickness,t_slot_length]);

            translate([-xtra/2, (padded_width-t_slot_width)/2, -(t_slot_length+thickness+2+0.7)])
            cube([padded_length+xtra,t_slot_width,thickness+2+0.7]);
            
        }
        
        // stuff to remove
        translate([thickness,thickness,thickness])
        cube([servo_length,servo_width,servo_height]);
        
        // for cables
        translate([servo_length+thickness,(padded_width-10)/2, thickness+2])
        cube([thickness+10, 10, 7]);
        
        // holes
        translate([-protuding_length+2.5+thickness,5+(3.7/2),servo_height-5])
        cylinder(5,d=3.7,center=false);
        
        translate([-protuding_length+2.5+thickness,5+(3.7/2)+10,servo_height-5])
        cylinder(5,d=3.7,center=false);
        
        translate([servo_length+protuding_length-2.5+thickness,5+(3.7/2),servo_height-5])
        cylinder(5,d=3.7,center=false);
        
        translate([servo_length+protuding_length-2.5+thickness,5+(3.7/2)+10,servo_height-5])
        cylinder(5,d=3.7,center=false);
        
        translate([padded_length/2,padded_width/2,-(t_slot_length+thickness+2+0.7)])
        cylinder(5,d=3.7,center=false);
    }
}

module motor_mount(){
    padded_width = mount_width+thickness*2;
    mount_length = 42;
   difference() {
     
     // stuff to build up
     union() {
        
         // motor mounting plate
         //translate([0,1,0])
        cube([motor_width,motor_width+2,thickness+1]);
         
         
        // Plate connected to the motor shaft
         padded_length = (orientation == 1) ? mount_length+thickness : mount_length
         ;
        padded_width = mount_width+thickness*2;
        translate([-mount_offset_x-thickness,-thickness,0])
          cube([padded_width,thickness,padded_length-17]);
         
         // back plate
         translate([-mount_offset_x-thickness,-thickness,padded_length-17])
         cube([padded_width,padded_width-thickness+2,thickness]);
         
        //t slot
         t_slot_length = 3+0.3;
         t_slot_width = 15-0.4;
         t_slot_thickness = 6-0.4;
         translate([padded_width-(t_slot_thickness/2)-10,-thickness,padded_length-17+thickness])
         cube([t_slot_thickness, padded_width-thickness+2, t_slot_length]);
         
         translate([padded_width-(t_slot_width/2)-10,-thickness,padded_length-17+thickness+t_slot_length])
         cube([t_slot_width, padded_width-thickness+2, thickness+2+0.7]);
         
        // motor side pieces for strength
        translate([motor_width,-thickness,0])
          cube([thickness,motor_width+thickness+2,padded_length-17]);
        translate([-thickness,-thickness,0])
          cube([thickness,motor_width+thickness+2,padded_length-17]);
        
        // angle connections on sides for strength
        tri_y = motor_width;
        tri_z = mount_length - ((orientation == 0) ? thickness : 0) - 25;
        translate([0,0,thickness])
          rotate([0,-90,0])
            linear_extrude(thickness)
              polygon( [ [0, tri_y], [tri_z, 0], [0, 0] ] );
        translate([motor_width+thickness,0,thickness])
          rotate([0,-90,0])
            linear_extrude(thickness)
              polygon( [ [0, tri_y], [tri_z, 0], [0, 0] ] );
        
        }   // orientation
     
    
     // and stuff to subtract
    
     // center motor hole
     translate([motor_width/2,motor_width/2,-1])
       cylinder(thickness+2,center_radius,center_radius);
       
     // screw
     translate([motor_width-6,motor_width/2,31])
        cylinder(4, d=3.8);
        
     translate([motor_width/2-6/2,motor_width/2+12,0])
        cube([6,12, thickness+2]);
     //translate([-mount_offset_x-thickness+2.5,-thickness,mount_length-17+2.5])
         //cube([mount_width+thickness*2-5,mount_width+thickness*2,16-5]);
    
     // bolt holes for motor
     translate([motor_width-bolt_inset,motor_width-bolt_inset,-1])
       cylinder(thickness+2,bolt_radius,bolt_radius);
     translate([motor_width-bolt_inset,bolt_inset,-1])
       cylinder(thickness+2,bolt_radius,bolt_radius);
     translate([bolt_inset,bolt_inset,-1])
       cylinder(thickness+2,bolt_radius,bolt_radius);
     translate([bolt_inset,motor_width-bolt_inset,-1])
       cylinder(thickness+2,bolt_radius,bolt_radius);
    
   }
    
}

module beam_mount() {
    openbeam_radius = openbeam_size/2;
    mount_length = temp_length;
    
   difference() {
     
     // stuff to build up
     union() {
        
        
        // plate on side along openbeam
        padded_length = (orientation == 1) ? mount_length+thickness : mount_length;
        padded_width = mount_width+thickness*2;
        translate([-mount_offset_x-thickness,-thickness-thickness-1,0])
          cube([padded_width,thickness,padded_length]);
         
        
        if (orientation == 0) {
            
          // openbeam connectors
          translate([(mount_width/2)-openbeam_radius-thickness-mount_offset_x,-openbeam_size-thickness-thickness-1,0])
            cube([thickness,openbeam_size,mount_length]);
          translate([(mount_width/2)+openbeam_radius-mount_offset_x,-openbeam_size-thickness-thickness-1,0])
            cube([thickness,openbeam_size,mount_length]);
            
          // openbeam slot (to limit twisting - also helps hold mount inplace while you add bolts)
          translate([(mount_width/2)-slot_diameter/2-mount_offset_x,-thickness-slot_depth-thickness-1,0])
            cube([slot_diameter,slot_depth,mount_length]);

          // hacky fillet for strength along openbeam
          fillet_size = max( 1, min( thickness-0.5, openbeam_radius-beam_bolt_radius-1) );
          fillet_x = (mount_width/2)-openbeam_radius-thickness-mount_offset_x;
          fillet_y = -thickness-fillet_size;
          linear_extrude(mount_length)
          translate([0,-thickness-1,0])
            polygon( [ [fillet_x, fillet_y], [fillet_x, -thickness], [fillet_x-fillet_size, -thickness] ] );

          fillet2_x = (mount_width/2)+openbeam_radius-mount_offset_x+thickness;
          linear_extrude(mount_length)
          translate([0,-thickness-1,0])
            polygon( [ [fillet2_x, fillet_y], [fillet2_x, -thickness], [fillet2_x+fillet_size, -thickness] ] );
          
          
          // angle connectors on top and bottom for strength
          tri2_y = -openbeam_size - thickness;
          tri2_x = (mount_width/2)-openbeam_radius-thickness-mount_offset_x;
          tri2_x2 = -mount_offset_x-thickness;
          linear_extrude(thickness)
          translate([0,-thickness-1,0])
            polygon( [ [tri2_x, tri2_y], [tri2_x, -thickness], [tri2_x2, -thickness] ] );
        
          tri2_x3 = (mount_width/2)+openbeam_radius-mount_offset_x+thickness;
          tri2_x4 = mount_width-mount_offset_x+thickness;
          linear_extrude(thickness)
          translate([0,-thickness-1,0])
            polygon( [ [tri2_x3, tri2_y], [tri2_x3, -thickness], [tri2_x4, -thickness] ] );

          // these make it difficult to FDM print without support, but are fine for powder prints
          if (false) {
            translate([0,0,mount_length-thickness])
              linear_extrude(thickness)
                polygon( [ [tri2_x, tri2_y], [tri2_x, -thickness], [tri2_x2, -thickness] ] );
            translate([0,0,mount_length-thickness])
              linear_extrude(thickness)
                polygon( [ [tri2_x3, tri2_y], [tri2_x3, -thickness], [tri2_x4, -thickness] ] );
          }
        
        } else {    // orientation
          
          // openbeam connectors
          translate([-thickness-mount_offset_x, -openbeam_size-thickness, (mount_length/2)-openbeam_radius-thickness/2])
            cube([padded_width, openbeam_size, thickness]);
          translate([-thickness-mount_offset_x,-openbeam_size-thickness,(mount_length/2)+openbeam_radius+thickness/2])
            cube([padded_width, openbeam_size, thickness]);

          // openbeam slot (to limit twisting - also helps hold mount inplace while you add bolts)
          translate([-thickness-mount_offset_x,-thickness-slot_depth,(mount_length/2)])
            cube([padded_width,slot_depth,slot_diameter]);
          
          // hacky fillet for strength along openbeam
          fillet_size = max( 1, min( thickness-0.5, openbeam_radius-beam_bolt_radius-1) );
          fillet_x = (mount_length/2)-openbeam_radius-thickness-mount_offset_z;
          fillet_y = -thickness-fillet_size;
          translate([-thickness-mount_offset_x,0,mount_length-mount_offset_z])
            rotate([0,90,0])
              linear_extrude(padded_width)
                polygon( [ [fillet_x, fillet_y], [fillet_x, -thickness], [fillet_x-fillet_size, -thickness] ] );

          fillet2_x = (mount_width/2)+openbeam_radius-mount_offset_x+thickness;
          translate([-thickness-mount_offset_x,0,mount_length+thickness-mount_offset_z])
            rotate([0,90,0])
              linear_extrude(padded_width)
                polygon( [ [fillet2_x, fillet_y], [fillet2_x, -thickness], [fillet2_x+fillet_size, -thickness] ] );

          // angle connectors on left and right, top and bottom for strength
          tri2_y = -openbeam_size - thickness;
          tri2_x = (mount_length/2)-openbeam_radius-thickness-mount_offset_z-thickness/2;
          tri2_x2 = -mount_offset_z-thickness;
          translate([-thickness-mount_offset_x,0,mount_length-mount_offset_z])
            rotate([0,90,0])
              linear_extrude(thickness)
                polygon( [ [tri2_x, tri2_y], [tri2_x, -thickness], [tri2_x2, -thickness] ] );
 
          tri2_x3 = (mount_length/2)+openbeam_radius-mount_offset_z+thickness+thickness/2;
          tri2_x4 = mount_length-mount_offset_z+thickness;
          translate([-thickness-mount_offset_x,0,mount_length+thickness-mount_offset_z])
            rotate([0,90,0])
              linear_extrude(thickness)
                polygon( [ [tri2_x3, tri2_y], [tri2_x3, -thickness], [tri2_x4, -thickness] ] );

          translate([mount_width-mount_offset_x,0,mount_length-mount_offset_z])
            rotate([0,90,0])
              linear_extrude(thickness)
                polygon( [ [tri2_x, tri2_y], [tri2_x, -thickness], [tri2_x2, -thickness] ] );

          translate([mount_width-mount_offset_x,0,mount_length+thickness-mount_offset_z])
            rotate([0,90,0])
              linear_extrude(thickness)
                polygon( [ [tri2_x3, tri2_y], [tri2_x3, -thickness], [tri2_x4, -thickness] ] );

        }   // orientation
     
     }
    
     // and stuff to subtract
    
     // center motor hole
     translate([motor_width/2,motor_width/2,-1])
       cylinder(thickness+2,center_radius,center_radius);
    
     // bolt holes for motor
     translate([motor_width-bolt_inset,motor_width-bolt_inset,-1])
       cylinder(thickness+2,bolt_radius,bolt_radius);
     translate([motor_width-bolt_inset,bolt_inset,-1])
       cylinder(thickness+2,bolt_radius,bolt_radius);
     translate([bolt_inset,bolt_inset,-1])
       cylinder(thickness+2,bolt_radius,bolt_radius);
     translate([bolt_inset,motor_width-bolt_inset,-1])
       cylinder(thickness+2,bolt_radius,bolt_radius);
    
     // bolt holes for openbeam
     if (orientation == 0) {
       end_offset = min( mount_length/3.25, max(2*thickness,2*beam_bolt_diameter) + beam_bolt_radius );
       translate([-thickness*2,-thickness-openbeam_size/2-thickness-1,end_offset])
        rotate([0,90,0])
          cylinder(mount_width+thickness*2,beam_bolt_radius,beam_bolt_radius);
       translate([-thickness*2,-thickness-openbeam_size/2-thickness-1,mount_length-end_offset])
        rotate([0,90,0])
          cylinder(mount_width+thickness*2,beam_bolt_radius,beam_bolt_radius);
     } else {
       end_offset = min( mount_width/3.25, max(2*thickness,2*beam_bolt_diameter) + beam_bolt_radius );
       translate([end_offset-mount_offset_x,-thickness-openbeam_size/2,0])
          cylinder(mount_length+thickness*2,beam_bolt_radius,beam_bolt_radius);
       translate([mount_width-end_offset-mount_offset_x,-thickness-openbeam_size/2,0])
          cylinder(mount_length+thickness*2,beam_bolt_radius,beam_bolt_radius);
     }
   }
}

module beam_base(){
   
   openbeam_radius = openbeam_size/2;
    
   difference() {
     
     // stuff to build up
     union() {
        
        
        // plate on side along openbeam
        padded_length = (orientation == 1) ? mount_length+thickness : mount_length;
        padded_width = mount_width+thickness*2;
        translate([-mount_offset_x-thickness,-thickness-thickness-1,0])
          cube([padded_width,thickness,padded_length]);
         
        
        if (orientation == 0) {
            
          // openbeam connectors
          translate([(mount_width/2)-openbeam_radius-thickness-mount_offset_x,-openbeam_size-thickness-thickness-1,0])
            cube([thickness,openbeam_size,mount_length]);
          translate([(mount_width/2)+openbeam_radius-mount_offset_x,-openbeam_size-thickness-thickness-1,0])
            cube([thickness,openbeam_size,mount_length]);
            
          // openbeam slot (to limit twisting - also helps hold mount inplace while you add bolts)
          translate([(mount_width/2)-slot_diameter/2-mount_offset_x,-thickness-slot_depth-thickness-1,0])
            cube([slot_diameter,slot_depth,mount_length]);

          // hacky fillet for strength along openbeam
          fillet_size = max( 1, min( thickness-0.5, openbeam_radius-beam_bolt_radius-1) );
          fillet_x = (mount_width/2)-openbeam_radius-thickness-mount_offset_x;
          fillet_y = -thickness-fillet_size;
          linear_extrude(mount_length)
          translate([0,-thickness-1,0])
            polygon( [ [fillet_x, fillet_y], [fillet_x, -thickness], [fillet_x-fillet_size, -thickness] ] );

          fillet2_x = (mount_width/2)+openbeam_radius-mount_offset_x+thickness;
          linear_extrude(mount_length)
          translate([0,-thickness-1,0])
            polygon( [ [fillet2_x, fillet_y], [fillet2_x, -thickness], [fillet2_x+fillet_size, -thickness] ] );
          
          
          // angle connectors on top and bottom for strength
          tri2_y = -openbeam_size - thickness;
          tri2_x = (mount_width/2)-openbeam_radius-thickness-mount_offset_x;
          tri2_x2 = -mount_offset_x-thickness;
          linear_extrude(thickness)
          translate([0,-thickness-1,0])
            polygon( [ [tri2_x, tri2_y], [tri2_x, -thickness], [tri2_x2, -thickness] ] );
        
          tri2_x3 = (mount_width/2)+openbeam_radius-mount_offset_x+thickness;
          tri2_x4 = mount_width-mount_offset_x+thickness;
          linear_extrude(thickness)
          translate([0,-thickness-1,0])
            polygon( [ [tri2_x3, tri2_y], [tri2_x3, -thickness], [tri2_x4, -thickness] ] );

          // these make it difficult to FDM print without support, but are fine for powder prints
          if (false) {
            translate([0,0,mount_length-thickness])
              linear_extrude(thickness)
                polygon( [ [tri2_x, tri2_y], [tri2_x, -thickness], [tri2_x2, -thickness] ] );
            translate([0,0,mount_length-thickness])
              linear_extrude(thickness)
                polygon( [ [tri2_x3, tri2_y], [tri2_x3, -thickness], [tri2_x4, -thickness] ] );
          }
        
        } else {    // orientation
          
          // openbeam connectors
          translate([-thickness-mount_offset_x, -openbeam_size-thickness, (mount_length/2)-openbeam_radius-thickness/2])
            cube([padded_width, openbeam_size, thickness]);
          translate([-thickness-mount_offset_x,-openbeam_size-thickness,(mount_length/2)+openbeam_radius+thickness/2])
            cube([padded_width, openbeam_size, thickness]);

          // openbeam slot (to limit twisting - also helps hold mount inplace while you add bolts)
          translate([-thickness-mount_offset_x,-thickness-slot_depth,(mount_length/2)])
            cube([padded_width,slot_depth,slot_diameter]);
          
          // hacky fillet for strength along openbeam
          fillet_size = max( 1, min( thickness-0.5, openbeam_radius-beam_bolt_radius-1) );
          fillet_x = (mount_length/2)-openbeam_radius-thickness-mount_offset_z;
          fillet_y = -thickness-fillet_size;
          translate([-thickness-mount_offset_x,0,mount_length-mount_offset_z])
            rotate([0,90,0])
              linear_extrude(padded_width)
                polygon( [ [fillet_x, fillet_y], [fillet_x, -thickness], [fillet_x-fillet_size, -thickness] ] );

          fillet2_x = (mount_width/2)+openbeam_radius-mount_offset_x+thickness;
          translate([-thickness-mount_offset_x,0,mount_length+thickness-mount_offset_z])
            rotate([0,90,0])
              linear_extrude(padded_width)
                polygon( [ [fillet2_x, fillet_y], [fillet2_x, -thickness], [fillet2_x+fillet_size, -thickness] ] );

          // angle connectors on left and right, top and bottom for strength
          tri2_y = -openbeam_size - thickness;
          tri2_x = (mount_length/2)-openbeam_radius-thickness-mount_offset_z-thickness/2;
          tri2_x2 = -mount_offset_z-thickness;
          translate([-thickness-mount_offset_x,0,mount_length-mount_offset_z])
            rotate([0,90,0])
              linear_extrude(thickness)
                polygon( [ [tri2_x, tri2_y], [tri2_x, -thickness], [tri2_x2, -thickness] ] );
 
          tri2_x3 = (mount_length/2)+openbeam_radius-mount_offset_z+thickness+thickness/2;
          tri2_x4 = mount_length-mount_offset_z+thickness;
          translate([-thickness-mount_offset_x,0,mount_length+thickness-mount_offset_z])
            rotate([0,90,0])
              linear_extrude(thickness)
                polygon( [ [tri2_x3, tri2_y], [tri2_x3, -thickness], [tri2_x4, -thickness] ] );

          translate([mount_width-mount_offset_x,0,mount_length-mount_offset_z])
            rotate([0,90,0])
              linear_extrude(thickness)
                polygon( [ [tri2_x, tri2_y], [tri2_x, -thickness], [tri2_x2, -thickness] ] );

          translate([mount_width-mount_offset_x,0,mount_length+thickness-mount_offset_z])
            rotate([0,90,0])
              linear_extrude(thickness)
                polygon( [ [tri2_x3, tri2_y], [tri2_x3, -thickness], [tri2_x4, -thickness] ] );

        }   // orientation
     
     }
    
     // and stuff to subtract
    
    
     // bolt holes for openbeam
     if (orientation == 0) {
       end_offset = min( mount_length/3.25, max(2*thickness,2*beam_bolt_diameter) + beam_bolt_radius );
       translate([-thickness*2,-thickness-openbeam_size/2-thickness-1,end_offset])
        rotate([0,90,0])
          cylinder(mount_width+thickness*2,beam_bolt_radius,beam_bolt_radius);
       translate([-thickness*2,-thickness-openbeam_size/2-thickness-1,mount_length-end_offset])
        rotate([0,90,0])
          cylinder(mount_width+thickness*2,beam_bolt_radius,beam_bolt_radius);
     } else {
       end_offset = min( mount_width/3.25, max(2*thickness,2*beam_bolt_diameter) + beam_bolt_radius );
       translate([end_offset-mount_offset_x,-thickness-openbeam_size/2,0])
          cylinder(mount_length+thickness*2,beam_bolt_radius,beam_bolt_radius);
       translate([mount_width-end_offset-mount_offset_x,-thickness-openbeam_size/2,0])
          cylinder(mount_length+thickness*2,beam_bolt_radius,beam_bolt_radius);
     }
   }
    
}