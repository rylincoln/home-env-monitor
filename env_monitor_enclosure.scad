// ============================================================
//  CC3220SF Home Environment Monitor — 3D Printable Enclosure
//  Parametric OpenSCAD Design
// ============================================================
//
//  Two-part snap-fit enclosure:
//    - BASE: holds CC3220SF LaunchPad, BME280, CCS811, BH1750,
//            MQ-7, and buzzer on M2.5 standoffs
//    - LID:  ventilation grille, light sensor window, buzzer
//            sound port, and snap clip receivers
//
//  Print settings:
//    Material: PETG (required — MQ-7 heater gets hot)
//    Layer:    0.2mm
//    Infill:   20% gyroid
//    Walls:    3 perimeters
//    Supports: yes (for USB cutout and snap clips)
//
//  Usage:
//    1. Render base:  set RENDER_PART = "base"
//    2. Render lid:   set RENDER_PART = "lid"
//    3. Render both:  set RENDER_PART = "both"  (exploded view)
//    4. Export each as STL separately for printing
// ============================================================

// === RENDER CONTROL ===
// Change to "base", "lid", or "both"
RENDER_PART = "both";

// === GLOBAL PARAMETERS ===
wall         = 2.0;     // Wall thickness (mm)
floor_t      = 2.0;     // Floor/ceiling thickness
corner_r     = 4.0;     // Corner radius
tolerance    = 0.3;     // Clearance for snap fits and inserts

// === ENCLOSURE OUTER DIMENSIONS ===
enc_w        = 140;     // Width (X)
enc_d        = 100;     // Depth (Y)
enc_h_base   = 24;      // Base height (including floor)
enc_h_lid    = 14;      // Lid height (including ceiling)
// Total closed height = enc_h_base + enc_h_lid = 38mm

// === DIVIDER (separates MCU and sensor compartments) ===
divider_x    = 82;      // X position of internal divider wall
divider_t    = 1.5;     // Divider wall thickness
wire_ch_w    = 14;      // Wire channel opening width in divider
wire_ch_z    = 16;      // Wire channel opening height

// === CC3220SF LAUNCHPAD ===
// Approximate board dimensions (measure your specific board)
lp_w         = 100;     // Board width
lp_d         = 58;      // Board depth
lp_hole_dx   = 92;      // Mounting hole spacing X
lp_hole_dy   = 50;      // Mounting hole spacing Y
lp_offset_x  = 8;       // Offset from left inner wall
lp_offset_y  = 10;      // Offset from front inner wall
lp_standoff_h = 6;      // Standoff height

// === USB CUTOUT ===
usb_w        = 12;      // USB micro-B width
usb_h        = 8;       // USB micro-B height
usb_z        = 8;       // Height from inner floor to USB center
usb_y_offset = 29;      // Y offset from front of enclosure

// === SENSOR BREAKOUT BOARDS ===
// BME280
bme_w        = 13;  bme_d = 10;
bme_hole_dx  = 8;   bme_hole_dy = 0;  // Single-axis mounting
bme_ox       = 92;  bme_oy = 10;      // Position in enclosure
bme_standoff = 4;

// CCS811
ccs_w        = 22;  ccs_d = 22;
ccs_hole_dx  = 17;  ccs_hole_dy = 17;
ccs_ox       = 110; ccs_oy = 10;
ccs_standoff = 4;

// BH1750
bh_w         = 22;  bh_d = 16;
bh_hole_dx   = 17;  bh_hole_dy = 11;
bh_ox        = 110; bh_oy = 72;
bh_standoff  = 4;

// MQ-7 (circular module)
mq7_dia      = 32;
mq7_hole_spacing = 28; // Mounting hole spacing
mq7_ox       = 102;    // Center X position
mq7_oy       = 52;     // Center Y position
mq7_standoff = 8;      // TALL standoff — heater clearance!

// === BUZZER ===
buzzer_dia   = 12;
buzzer_ox    = 40;      // Center X in enclosure
buzzer_oy    = 75;      // Center Y in enclosure

// === M2.5 HARDWARE ===
m25_hole_d   = 2.5 + tolerance;   // Through-hole for M2.5 screw
m25_insert_d = 3.5;               // Hole for heat-set insert (pre-melt)
m25_insert_h = 4.0;               // Insert depth
standoff_od  = 6.0;               // Standoff outer diameter

// === SNAP CLIPS ===
clip_w       = 8;       // Clip width
clip_h       = 8;       // Clip hook depth into lid
clip_t       = 1.5;     // Clip material thickness
clip_hook    = 1.2;     // Hook overhang
clip_inset   = 20;      // Distance from corners

// === VENTILATION GRILLE (lid) ===
vent_slot_w  = 2.5;     // Individual slot width
vent_slot_l  = 40;      // Slot length
vent_spacing = 5;       // Center-to-center spacing
vent_count   = 8;       // Number of slots
vent_ox      = 88;      // Start X position (above sensor bay)
vent_oy      = 8;       // Start Y position

// === LIGHT WINDOW (lid) ===
light_win_w  = 16;
light_win_d  = 20;
// Positioned above BH1750

// === BUZZER SOUND PORT (lid) ===
sound_port_dia = 8;
// Array of small holes for speaker grille effect
sound_hole_dia = 2;
sound_hole_count = 7;   // Hex pattern

// === WALL MOUNT KEYHOLES ===
keyhole_large_d  = 8;   // Screw head diameter
keyhole_small_d  = 4;   // Screw shaft slot width
keyhole_slot_len = 8;   // Slot length
keyhole_spacing  = 70;  // Distance apart
keyhole_y        = 50;  // Y center

// ============================================================
//  MODULES
// ============================================================

// Rounded rectangle (2D profile for extrusion)
module rounded_rect(w, d, r) {
    offset(r) offset(-r)
        square([w, d], center=false);
}

// Standoff post with hole for heat-set insert
module standoff(h, od, insert_d, insert_h) {
    difference() {
        cylinder(h=h, d=od, $fn=24);
        translate([0, 0, h - insert_h])
            cylinder(h=insert_h + 0.1, d=insert_d, $fn=24);
    }
}

// Snap clip (on base)
module snap_clip() {
    // Flexible arm
    cube([clip_t, clip_w, clip_h]);
    // Hook
    translate([clip_t, 0, clip_h - clip_hook])
        cube([clip_hook, clip_w, clip_hook]);
}

// Snap clip receiver slot (in lid wall)
module snap_clip_receiver() {
    translate([-tolerance, -tolerance, 0])
        cube([clip_t + clip_hook + tolerance*2,
              clip_w + tolerance*2,
              clip_h + tolerance]);
}

// Keyhole slot (for wall mounting)
module keyhole(large_d, small_d, slot_len) {
    // Large circle (screw head entry)
    cylinder(h=floor_t + 0.2, d=large_d, $fn=32, center=true);
    // Slot upward
    translate([0, slot_len/2, 0])
        cylinder(h=floor_t + 0.2, d=small_d, $fn=24, center=true);
    // Connect slot
    translate([-small_d/2, 0, -(floor_t+0.2)/2])
        cube([small_d, slot_len/2, floor_t + 0.2]);
}

// ============================================================
//  BASE
// ============================================================
module base() {
    difference() {
        union() {
            // Main shell
            difference() {
                // Outer body
                linear_extrude(enc_h_base)
                    rounded_rect(enc_w, enc_d, corner_r);
                // Hollow inside
                translate([wall, wall, floor_t])
                    linear_extrude(enc_h_base)
                        rounded_rect(enc_w - wall*2, enc_d - wall*2, corner_r - wall);
            }

            // === INTERNAL DIVIDER ===
            translate([divider_x, wall, floor_t])
                difference() {
                    cube([divider_t, enc_d - wall*2, enc_h_base - floor_t]);
                    // Wire channel opening
                    translate([-0.1,
                               (enc_d - wall*2)/2 - wire_ch_w/2,
                               0])
                        cube([divider_t + 0.2, wire_ch_w, wire_ch_z]);
                }

            // === LAUNCHPAD STANDOFFS ===
            for (dx = [0, lp_hole_dx], dy = [0, lp_hole_dy]) {
                translate([wall + lp_offset_x + dx + (lp_w - lp_hole_dx)/2,
                           wall + lp_offset_y + dy + (lp_d - lp_hole_dy)/2,
                           floor_t])
                    standoff(lp_standoff_h, standoff_od, m25_insert_d, m25_insert_h);
            }

            // === BME280 STANDOFFS ===
            for (dx = [0, bme_hole_dx]) {
                translate([wall + bme_ox + dx,
                           wall + bme_oy + bme_d/2,
                           floor_t])
                    standoff(bme_standoff, standoff_od, m25_insert_d, m25_insert_h);
            }

            // === CCS811 STANDOFFS ===
            for (dx = [0, ccs_hole_dx], dy = [0, ccs_hole_dy]) {
                translate([wall + ccs_ox + dx,
                           wall + ccs_oy + dy,
                           floor_t])
                    standoff(ccs_standoff, standoff_od, m25_insert_d, m25_insert_h);
            }

            // === BH1750 STANDOFFS ===
            for (dx = [0, bh_hole_dx], dy = [0, bh_hole_dy]) {
                translate([wall + bh_ox + dx,
                           wall + bh_oy + dy,
                           floor_t])
                    standoff(bh_standoff, standoff_od, m25_insert_d, m25_insert_h);
            }

            // === MQ-7 TALL STANDOFFS ===
            for (dx = [-mq7_hole_spacing/2, mq7_hole_spacing/2]) {
                translate([wall + mq7_ox + dx,
                           wall + mq7_oy,
                           floor_t])
                    standoff(mq7_standoff, standoff_od, m25_insert_d, m25_insert_h);
            }

            // === SNAP CLIPS (on top of base walls) ===
            // Front-left
            translate([wall - clip_t, clip_inset, enc_h_base])
                snap_clip();
            // Front-right
            translate([enc_w - wall, clip_inset, enc_h_base])
                snap_clip();
            // Back-left
            translate([wall - clip_t, enc_d - clip_inset - clip_w, enc_h_base])
                snap_clip();
            // Back-right
            translate([enc_w - wall, enc_d - clip_inset - clip_w, enc_h_base])
                snap_clip();
        }

        // === USB CUTOUT (left wall) ===
        translate([-0.1,
                   wall + lp_offset_y + usb_y_offset - usb_w/2,
                   floor_t + usb_z - usb_h/2])
            cube([wall + 0.2, usb_w, usb_h]);

        // === WALL MOUNT KEYHOLES (bottom) ===
        translate([enc_w/2 - keyhole_spacing/2, keyhole_y, floor_t/2])
            keyhole(keyhole_large_d, keyhole_small_d, keyhole_slot_len);
        translate([enc_w/2 + keyhole_spacing/2, keyhole_y, floor_t/2])
            keyhole(keyhole_large_d, keyhole_small_d, keyhole_slot_len);
    }
}

// ============================================================
//  LID
// ============================================================
module lid() {
    difference() {
        union() {
            // Main lid shell (upside down for printing, we'll flip later)
            difference() {
                linear_extrude(enc_h_lid)
                    rounded_rect(enc_w, enc_d, corner_r);
                // Hollow
                translate([wall, wall, floor_t])
                    linear_extrude(enc_h_lid)
                        rounded_rect(enc_w - wall*2, enc_d - wall*2, corner_r - wall);
            }

            // Inner lip (slides inside base walls)
            lip_h = 4;
            lip_inset = wall + tolerance;
            translate([lip_inset, lip_inset, enc_h_lid - 0.01])
                linear_extrude(lip_h)
                    difference() {
                        rounded_rect(enc_w - lip_inset*2, enc_d - lip_inset*2, corner_r - lip_inset);
                        offset(-wall)
                            rounded_rect(enc_w - lip_inset*2, enc_d - lip_inset*2, corner_r - lip_inset);
                    }

            // Buzzer recess ring (inside lid)
            translate([wall + buzzer_ox, wall + buzzer_oy, floor_t])
                difference() {
                    cylinder(h=3, d=buzzer_dia + 4, $fn=36);
                    cylinder(h=3.1, d=buzzer_dia + tolerance*2, $fn=36);
                }
        }

        // === VENTILATION GRILLE ===
        for (i = [0 : vent_count - 1]) {
            translate([wall + vent_ox + i * vent_spacing,
                       wall + vent_oy,
                       -0.1])
                cube([vent_slot_w, vent_slot_l, floor_t + 0.2]);
        }

        // === LIGHT SENSOR WINDOW ===
        translate([wall + bh_ox + bh_hole_dx/2 - light_win_w/2,
                   wall + bh_oy + bh_hole_dy/2 - light_win_d/2,
                   -0.1])
            cube([light_win_w, light_win_d, floor_t + 0.2]);

        // === BUZZER SOUND PORT (hex pattern) ===
        translate([wall + buzzer_ox, wall + buzzer_oy, -0.1]) {
            // Center hole
            cylinder(h=floor_t + 0.2, d=sound_hole_dia, $fn=16);
            // Ring of holes
            for (a = [0 : 60 : 359]) {
                translate([3.5 * cos(a), 3.5 * sin(a), 0])
                    cylinder(h=floor_t + 0.2, d=sound_hole_dia, $fn=16);
            }
        }

        // === SNAP CLIP RECEIVER SLOTS ===
        // Must align with base clip positions
        // Front-left
        translate([wall - clip_t - tolerance,
                   clip_inset - tolerance,
                   enc_h_lid - clip_h - tolerance])
            snap_clip_receiver();
        // Front-right
        translate([enc_w - wall - clip_hook - tolerance,
                   clip_inset - tolerance,
                   enc_h_lid - clip_h - tolerance])
            snap_clip_receiver();
        // Back-left
        translate([wall - clip_t - tolerance,
                   enc_d - clip_inset - clip_w - tolerance,
                   enc_h_lid - clip_h - tolerance])
            snap_clip_receiver();
        // Back-right
        translate([enc_w - wall - clip_hook - tolerance,
                   enc_d - clip_inset - clip_w - tolerance,
                   enc_h_lid - clip_h - tolerance])
            snap_clip_receiver();
    }
}

// ============================================================
//  RENDER
// ============================================================

if (RENDER_PART == "base") {
    base();
}
else if (RENDER_PART == "lid") {
    // Print lid upside down (ceiling on build plate)
    translate([0, 0, enc_h_lid])
        rotate([180, 0, 0])
            translate([0, -enc_d, 0])
                lid();
}
else if (RENDER_PART == "both") {
    // Exploded view for visualization
    color("#1a2332", 0.85) base();
    color("#22d3ee", 0.4)
        translate([0, 0, enc_h_base + 15])  // 15mm gap for visibility
            lid();
}
