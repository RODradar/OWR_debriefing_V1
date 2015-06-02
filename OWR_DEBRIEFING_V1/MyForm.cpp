#include "MyForm.h"

#using <System.dll>
#using <System.Windows.Forms.dll>
#using <System.Drawing.dll>
using namespace System;
using namespace System::Drawing;
using namespace System::Windows::Forms;


namespace ROD_OMR_V1
{
	// --------------------------------------------------
	//  MyForm  
	//  Alon Slapak 13/2/2015
	// 	Description:	Constructor
	// 	Reference: 
	//  Input value:
	//  Return Value: 
	// --------------------------------------------------

	MyForm::MyForm(void)
	{
		InitializeComponent();
		//
		//TODO: Add the constructor code here
		//
		//-----------------------------------------
		//	Initialize Radar detections array
		//-----------------------------------------
		Radar_detections_array = gcnew array<T_Target>(MAX_DETECTIONS);
		//-----------------------------------------
		//	Initialize error function array
		//-----------------------------------------
		Error_function_array = gcnew array<double>(SCREEN_SIZE_Y);
		B_ERROR_FUNCTION->Series["error_function"]->Points->AddXY(0, 0);
		//-----------------------------------------
		//	Initialize Obstacles map true
		//-----------------------------------------
		OBSTACLES_map_true.Obstacles = gcnew array<T_Obstacle>(MAX_OBSTACLES);
		OBSTACLES_map_true.number_of_obstacles = 0;
		//-----------------------------------------
		//	Initialize Obstacles  estimated
		//-----------------------------------------
		OBSTACLES_map_estimated.Obstacles = gcnew array<T_Obstacle>(MAX_OBSTACLES);
		OBSTACLES_map_estimated.number_of_obstacles = 0;
		//-----------------------------------------
		//	Read INITIAL DATA from file
		//-----------------------------------------
		INITIAL_data = (T_INITIAL_data*)malloc(sizeof(T_INITIAL_data));
		fstream file_handle = fstream(INITIAL_VALUES_FILE_NAME, ios::in | ios::binary);
		int	begin = (int)file_handle.tellg();
		file_handle.seekg(0, ios::end);
		int end = (int)file_handle.tellg();
		file_handle.seekg(0, ios::beg);
		//  if file length is equal to structure length
		int size_of_data = sizeof(T_INITIAL_data);
		if (end - begin == size_of_data)
		{
			file_handle.read((char*)INITIAL_data, sizeof(T_INITIAL_data));
		}
		else
		{
			sprintf_s(INITIAL_data->Obstackes_file_name, "Enter obstacles file name...............");
			INITIAL_data->Variance_position_wires = 50;
			INITIAL_data->False_alarm_rate = 1;
			INITIAL_data->radar_range = 1500;
			INITIAL_data->radar_FOV = 180;
			INITIAL_data->radar_refresh_time = 1;
			INITIAL_data->screen_width = 15;
			INITIAL_data->Helicopter_Speed = 50;
			INITIAL_data->route_new = true;
			INITIAL_data->show_detections = true;
			INITIAL_data->max_range_error_meter = MAX_RANGE_ERROR;
			INITIAL_data->reliability_threshold = RELIABILITY_THRESHOLD;
			INITIAL_data->wire_segment_length_meter = WIRE_SEGMENT_LENGTH_METER;
			INITIAL_data->GPS_latitude = (float)MAP_BASE_LAT;
			INITIAL_data->GPS_longitude = (float)MAP_BASE_LON;
		}
		file_handle.close();
		// --------------------------------------------------
		//			Load true obstacles map (TOM)
		// --------------------------------------------------
		Obstacles_map_load(OBSTACLES_map_true, INITIAL_data->Obstackes_file_name);
		//-----------------------------------------
		//	Initialize Helicopter route
		//-----------------------------------------
		Start_wire_from_mouse = false;
		Flight_expeiment_initialized = false;
		HELICOPTER_route_master.Helicopter_initial_angle = 0;
		HELICOPTER_route_master.Helicopter_delta_angle = 0;
		HELICOPTER_route_master.Helicopter_position = convert_Screen_to_UTM(PointF(SCREEN_SIZE_X / 2, SCREEN_SIZE_Y - 1));
		//-----------------------------------------
		//		Init GUI -  generator
		//-----------------------------------------
		B_HELICOPTER_SPEED->Text = INITIAL_data->Helicopter_Speed.ToString();
		B_WIRE_VARIANCE->Text = INITIAL_data->Variance_position_wires.ToString();
		B_FALSE_ALARM_RATE->Text = INITIAL_data->False_alarm_rate.ToString();
		B_RADAR_RANGE->Text = INITIAL_data->radar_range.ToString();
		B_RADAR_FOV->Text = INITIAL_data->radar_FOV.ToString();
		B_RADAR_REFRESH->Text = INITIAL_data->radar_refresh_time.ToString();
		B_SCREEN_WIDTH->Value = INITIAL_data->screen_width;
		B_ROUTE_NEW->Checked = INITIAL_data->route_new;
		B_ROUTE_REPEAT->Checked = !INITIAL_data->route_new;
		double		scale = (double)B_SCREEN_WIDTH->Value / 10;
		B_SCALE->Text = scale.ToString();
		B_OBSTACLES_file->Text = gcnew String(INITIAL_data->Obstackes_file_name);
		B_PLOT_DETECTIONS->Checked = INITIAL_data->show_detections;
		B_MAX_ERROR_RANGE->Text = INITIAL_data->max_range_error_meter.ToString();
		B_RELIABILITY_THRESHOLD->Text = INITIAL_data->reliability_threshold.ToString();
		B_WIRE_SEGMENT_LENGTH_METER->Text = INITIAL_data->wire_segment_length_meter.ToString();
		B_GPS_LAT->Text = INITIAL_data->GPS_latitude.ToString("N6");
		B_GPS_LONG->Text = INITIAL_data->GPS_longitude.ToString("N6");
		//-----------------------------------------
		//		Init GUI - Viewer
		//-----------------------------------------
		B_FILE_TOM->Text = gcnew String(INITIAL_data->Obstackes_file_name);
	}

	// --------------------------------------------------
	//  Obstacles_map_plot  
	//  Alon Slapak 13/2/2015
	// 	Description:	plots the obstacles map on the screen
	// 	Reference: 
	//  Input value:
	//  Return Value: 
	// --------------------------------------------------
	void MyForm::Obstacles_map_plot(T_OBSTACLES_map obstacles_map, Color pen_Color)
	{

		Pen^		pen = gcnew Pen(pen_Color);
		Graphics ^	graphics = B_PANEL->CreateGraphics();
		int			i;
		int			pylon_position_shift = (int)(PYLON_SIZE_TRUE / 2);
		PointF		obstacle_Screen_1, obstacle_Screen_2;
		//-------------------------------------------------------
		//	Plot the obstacles map
		//-------------------------------------------------------
		for (i = 0; i < obstacles_map.number_of_obstacles; i++)
		{
			if (obstacles_map.Obstacles[i].Obstacle_reliability > INITIAL_data->reliability_threshold)
			{
				//-------------------------------------------------------
				//	Plot pylon
				//-------------------------------------------------------
				if (obstacles_map.Obstacles[i].Obstacle_type == OBSTACLE_PYLON)
				{
					obstacle_Screen_1 = convert_UTM_to_Screen(obstacles_map.Obstacles[i].Point_1);
					graphics->DrawRectangle(pen, (int)obstacle_Screen_1.X - pylon_position_shift, (int)obstacle_Screen_1.Y - pylon_position_shift, PYLON_SIZE_TRUE, PYLON_SIZE_TRUE);
				}
				//-------------------------------------------------------
				//	Plot wire
				//-------------------------------------------------------
				if (obstacles_map.Obstacles[i].Obstacle_type == OBSTACLE_WIRE)
				{
					obstacle_Screen_1 = convert_UTM_to_Screen(obstacles_map.Obstacles[i].Point_1);
					obstacle_Screen_2 = convert_UTM_to_Screen(obstacles_map.Obstacles[i].Point_2);
					graphics->DrawLine(pen, obstacle_Screen_1, obstacle_Screen_2);
				}
			}
		}
		delete graphics;
	}
	//// --------------------------------------------------
	////  Obstacles_map_estimated_plot  
	////  Alon Slapak 13/2/2015
	//// 	Description:	plots the obstacles map on the screen
	//// 	Reference: 
	////  Input value:
	////  Return Value: 
	//// --------------------------------------------------
	//void MyForm::Obstacles_map_estimated_plot()
	//{

	//	Pen^		pen = gcnew Pen(COLOR_ESTIMATED);
	//	Graphics ^	graphics = B_PANEL->CreateGraphics();
	//	int			i;
	//	int			pylon_position_shift = (int)(PYLON_SIZE_ESTIMATED / 2);
	//	//-------------------------------------------------------
	//	//	Plot the wire system
	//	//-------------------------------------------------------
	//	for (i = 0; i < OBSTACLES_map_estimated.number_of_obstacles; i++)
	//	{
	//		if (OBSTACLES_map_estimated.Obstacles[i].Obstacle_reliability > INITIAL_data->reliability_threshold)
	//		{
	//			//-------------------------------------------------------
	//			//	Plot pylon
	//			//-------------------------------------------------------
	//			if (OBSTACLES_map_estimated.Obstacles[i].Obstacle_type == OBSTACLE_PYLON)
	//			{
	//				graphics->DrawRectangle(pen, (int)OBSTACLES_map_estimated.Obstacles[i].Point_1.X - pylon_position_shift, (int)OBSTACLES_map_estimated.Obstacles[i].Point_1.Y - pylon_position_shift, PYLON_SIZE_ESTIMATED, PYLON_SIZE_ESTIMATED);
	//			}
	//			//-------------------------------------------------------
	//			//	Plot wire
	//			//-------------------------------------------------------
	//			if (OBSTACLES_map_estimated.Obstacles[i].Obstacle_type == OBSTACLE_WIRE)
	//			{
	//				graphics->DrawLine(pen, OBSTACLES_map_estimated.Obstacles[i].Point_1, OBSTACLES_map_estimated.Obstacles[i].Point_2);
	//			}
	//		}
	//	}
	//	delete graphics;
	//}
	// --------------------------------------------------
	//  B_PLOT_DETECTIONS_CheckedChanged  
	//  Alon Slapak 22/4/2015
	// 	Description:	Show/Hide detections on screen
	// 	Reference: 
	//  Input value:
	//  Return Value: 
	// --------------------------------------------------
	void MyForm::B_PLOT_DETECTIONS_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
	{
		INITIAL_data->show_detections = B_PLOT_DETECTIONS->Checked;
	}
	// --------------------------------------------------
	//  B_PANEL_MouseMove  
	//  Alon Slapak 24/4/2015
	// 	Description:	Get wires from user using mouse
	// 	Reference: 
	//  Input value:
	//  Return Value: 
	// --------------------------------------------------
	void MyForm::B_PANEL_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
	{
		Pen^		pen_on = gcnew Pen(Color::Orange);
		Pen^		pen_off = gcnew Pen(Color::Black);
		Graphics ^	graphics = B_PANEL->CreateGraphics();

		if (Start_wire_from_mouse)
		{
			graphics->DrawLine(pen_off, Last_screen_point_of_wire, Point_mouse_move);
			Point_mouse_move.X = (float)e->X;
			Point_mouse_move.Y = (float)e->Y;
			graphics->DrawLine(pen_on, Last_screen_point_of_wire, Point_mouse_move);
		}
		delete graphics;
	}
	// --------------------------------------------------
	//  B_PANEL_MouseLeave  
	//  Alon Slapak 24/4/2015
	// 	Description:	Get wires from user using mouse
	// 	Reference: 
	//  Input value:
	//  Return Value: 
	// --------------------------------------------------
	void MyForm::B_PANEL_MouseLeave(System::Object^  sender, System::EventArgs^  e)
	{
		//-------------------------------------
		//		Erase line
		//-------------------------------------
		Graphics ^	graphics = B_PANEL->CreateGraphics();
		Pen^		pen_off = gcnew Pen(Color::Black);
		graphics->DrawLine(pen_off, Last_screen_point_of_wire, Point_mouse_move);
		delete graphics;
		Start_wire_from_mouse = false;
	}
	// --------------------------------------------------
	//  B_PANEL_MouseClick  
	//  Alon Slapak 13/2/2015
	// 	Description:	Get wires from user using mouse
	// 	Reference: 
	//  Input value:
	//  Return Value: 
	// --------------------------------------------------
	void MyForm::B_PANEL_MouseClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
	{
		//-------------------------------------------------------
		//	LEFT: add WIRE
		//-------------------------------------------------------
		if (e->Button == ::MouseButtons::Left)
		{
			if (Start_wire_from_mouse == true)
			{
				OBSTACLES_map_true.Obstacles[OBSTACLES_map_true.number_of_obstacles].Obstacle_type = OBSTACLE_WIRE;
				OBSTACLES_map_true.Obstacles[OBSTACLES_map_true.number_of_obstacles].Point_1 = convert_Screen_to_UTM(Last_screen_point_of_wire);
				OBSTACLES_map_true.Obstacles[OBSTACLES_map_true.number_of_obstacles].Point_2 = convert_Screen_to_UTM(PointF((float)e->X, (float)e->Y));
				OBSTACLES_map_true.Obstacles[OBSTACLES_map_true.number_of_obstacles].Obstacle_reliability = 100;
				OBSTACLES_map_true.number_of_obstacles++;
			}
			OBSTACLES_map_true.Obstacles[OBSTACLES_map_true.number_of_obstacles].Obstacle_type = OBSTACLE_PYLON;
			OBSTACLES_map_true.Obstacles[OBSTACLES_map_true.number_of_obstacles].Point_1 = convert_Screen_to_UTM(PointF((float)e->X, (float)e->Y));
			OBSTACLES_map_true.Obstacles[OBSTACLES_map_true.number_of_obstacles].Obstacle_reliability = 100;
			OBSTACLES_map_true.number_of_obstacles++;
			Last_screen_point_of_wire = PointF((float)e->X, (float)e->Y);
			Point_mouse_move = PointF((float)e->X, (float)e->Y);
			Start_wire_from_mouse = true;
			//-------------------------------------------------------
			//	Refresh wires and pylons plot
			//-------------------------------------------------------
			Obstacles_map_plot(OBSTACLES_map_true, COLOR_TRUE);
		}
		//-------------------------------------------------------
		//	RIGHT: add Pylon
		//-------------------------------------------------------
		if (e->Button == ::MouseButtons::Right)
		{
			OBSTACLES_map_true.Obstacles[OBSTACLES_map_true.number_of_obstacles].Obstacle_type = OBSTACLE_PYLON;
			OBSTACLES_map_true.Obstacles[OBSTACLES_map_true.number_of_obstacles].Point_1 = convert_Screen_to_UTM(PointF((float)e->X, (float)e->Y));
			OBSTACLES_map_true.Obstacles[OBSTACLES_map_true.number_of_obstacles].Obstacle_reliability = 100;
			OBSTACLES_map_true.number_of_obstacles++;
			//-------------------------------------------------------
			//	Refresh wires and pylons plot
			//-------------------------------------------------------
			Obstacles_map_plot(OBSTACLES_map_true, COLOR_TRUE);
		}
	}

	// --------------------------------------------------
	//  B_CLEAR_WIRE_Click  
	//  Alon Slapak 13/2/2015
	// 	Description:	Clear wires
	// 	Reference: 
	//  Input value:
	//  Return Value: 
	// --------------------------------------------------
	void MyForm::B_CLEAR_WIRE_Click(System::Object^  sender, System::EventArgs^  e)
	{
		OBSTACLES_map_true.number_of_obstacles = 0;
		B_PANEL->Invalidate();
	}
	// --------------------------------------------------
	//  MyForm_FormClosing  
	//  Alon Slapak 13/2/2015
	// 	Description:	Serialization
	// 	Reference: 
	//  Input value:
	//  Return Value: 
	// --------------------------------------------------
	void MyForm::MyForm_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e)
	{
		//---------------------------------------------------
		//		Open file for serialization of GUI data
		//---------------------------------------------------
		fstream file_handle = fstream(INITIAL_VALUES_FILE_NAME, ios::out | ios::binary);
		pin_ptr<int> p1 = &(INITIAL_data->Helicopter_Speed);
		file_handle.write((char*)(p1), sizeof(T_INITIAL_data));
		file_handle.close();
	}
	// --------------------------------------------------
	//  B_PANEL_Paint  
	//  Alon Slapak 13/2/2015
	// 	Description:	Serialization
	// 	Reference: 
	//  Input value:
	//  Return Value: 
	// --------------------------------------------------
	void MyForm::B_PANEL_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e)
	{
		Obstacles_map_plot(OBSTACLES_map_true, COLOR_TRUE);
	}

	// --------------------------------------------------
	//  B_SPEED_ValueChanged  
	//  Alon Slapak 13/2/2015
	// 	Description:	change speed parameter
	// 	Reference: 
	//  Input value:
	//  Return Value: 
	// --------------------------------------------------
	void MyForm::B_SPEED_ValueChanged(System::Object^  sender, System::EventArgs^  e)
	{
		INITIAL_data->Helicopter_Speed = System::Convert::ToInt32(B_HELICOPTER_SPEED->Value);
	}
	// --------------------------------------------------
	//  B_WIRE_VARIANCE_ValueChanged  
	//  Alon Slapak 13/2/2015
	// 	Description:	change variance of position parameter
	// 	Reference: 
	//  Input value:
	//  Return Value: 
	// --------------------------------------------------
	void MyForm::B_WIRE_VARIANCE_ValueChanged(System::Object^  sender, System::EventArgs^  e)
	{
		INITIAL_data->Variance_position_wires = System::Convert::ToInt32(B_WIRE_VARIANCE->Value);
	}
	// --------------------------------------------------
	//  B_FALSE_ALARM_RATE_ValueChanged  
	//  Alon Slapak 13/2/2015
	// 	Description:	change false alarm rate parameter
	// 	Reference: 
	//  Input value:
	//  Return Value: 
	// --------------------------------------------------
	void MyForm::B_FALSE_ALARM_RATE_ValueChanged(System::Object^  sender, System::EventArgs^  e)
	{
		INITIAL_data->False_alarm_rate = System::Convert::ToInt32(B_FALSE_ALARM_RATE->Value);
	}
	// --------------------------------------------------
	//  B_RADAR_RANGE_ValueChanged  
	//  Alon Slapak 18/2/2015
	// 	Description:	change radar range
	// 	Reference: 
	//  Input value:
	//  Return Value: 
	// --------------------------------------------------
	void MyForm::B_RADAR_RANGE_ValueChanged(System::Object^  sender, System::EventArgs^  e)
	{
		INITIAL_data->radar_range = System::Convert::ToInt32(B_RADAR_RANGE->Value);
	}
	// --------------------------------------------------
	//  B_RADAR_FOV_ValueChanged  
	//  Alon Slapak 18/2/2015
	// 	Description:	change radar FOV
	// 	Reference: 
	//  Input value:
	//  Return Value: 
	// --------------------------------------------------
	void MyForm::B_RADAR_FOV_ValueChanged(System::Object^  sender, System::EventArgs^  e)
	{
		INITIAL_data->radar_FOV = System::Convert::ToInt32(B_RADAR_FOV->Value);
	}
	// --------------------------------------------------
	//  B_RADAR_REFRESH_ValueChanged  
	//  Alon Slapak 18/2/2015
	// 	Description:	change radar refresh time
	// 	Reference: 
	//  Input value:
	//  Return Value: 
	// --------------------------------------------------
	void MyForm::B_RADAR_REFRESH_ValueChanged(System::Object^  sender, System::EventArgs^  e)
	{
		INITIAL_data->radar_refresh_time = System::Convert::ToInt32(B_RADAR_REFRESH->Value);
	}

	// --------------------------------------------------
	//  B_GPS_LAT_TextChanged  
	//  Alon Slapak 10/5/2015
	// 	Description:	Enter GPS latitude of helicopter start position
	// 	Reference: 
	//  Input value:
	//  Return Value: 
	// --------------------------------------------------
	void MyForm::B_GPS_LAT_TextChanged(System::Object^  sender, System::EventArgs^  e)
	{
		INITIAL_data->GPS_latitude = (float)System::Convert::ToDouble(B_GPS_LAT->Text);
	}
	// --------------------------------------------------
	//  B_GPS_LONG_TextChanged  
	//  Alon Slapak 10/5/2015
	// 	Description:	Enter GPS longitude of helicopter start position
	// 	Reference: 
	//  Input value:
	//  Return Value: 
	// --------------------------------------------------
	void MyForm::B_GPS_LONG_TextChanged(System::Object^  sender, System::EventArgs^  e)
	{
		INITIAL_data->GPS_longitude = (float)System::Convert::ToDouble(B_GPS_LONG->Text);
	}
	// --------------------------------------------------
	//  B_FILE_DETECTIONS_GENERATOR_TextChanged  
	//  Alon Slapak 17/5/2015
	// 	Description:	Enter detection 
	// 	Reference: 
	//  Input value:
	//  Return Value: 
	// --------------------------------------------------
	void MyForm::B_FILE_DETECTIONS_GENERATOR_TextChanged(System::Object^  sender, System::EventArgs^  e)
	{

	}

	// --------------------------------------------------
	//  B_SCREEN_WIDTH_Scroll  
	//  Alon Slapak 18/2/2015
	// 	Description:	change radar refresh time
	// 	Reference: 
	//  Input value:
	//  Return Value: 
	// --------------------------------------------------
	void MyForm::B_SCREEN_WIDTH_Scroll(System::Object^  sender, System::EventArgs^  e)
	{
		double		scale = (double)B_SCREEN_WIDTH->Value / 10;
		INITIAL_data->screen_width = B_SCREEN_WIDTH->Value;
		B_SCALE->Text = scale.ToString();
		// --------------------------------------------------
		// Refresh screen with new scale
		// --------------------------------------------------
		B_PANEL->Invalidate();
		Obstacles_map_plot(OBSTACLES_map_true, COLOR_TRUE);
	}
	// --------------------------------------------------
	//  B_ROUTE_NEW_CheckedChanged  
	//  Alon Slapak 20/2/2015
	// 	Description:	Select new route or repeat previous
	// 	Reference: 
	//  Input value:
	//  Return Value: 
	// --------------------------------------------------
	void MyForm::B_ROUTE_NEW_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
	{
		INITIAL_data->route_new = B_ROUTE_NEW->Checked;
	}
	// --------------------------------------------------
	//  B_RELIABILITY_THRESHOLD_ValueChanged  
	//  Alon Slapak 23/4/2015
	// 	Description:	Obstacle map estimation: change reliability threshold
	// 	Reference: 
	//  Input value:
	//  Return Value: 
	// --------------------------------------------------
	void MyForm::B_RELIABILITY_THRESHOLD_ValueChanged(System::Object^  sender, System::EventArgs^  e)
	{
		INITIAL_data->reliability_threshold = System::Convert::ToInt32(B_RELIABILITY_THRESHOLD->Value);
	}
	// --------------------------------------------------
	//  B_MAX_ERROR_RANGE_ValueChanged  
	//  Alon Slapak 23/4/2015
	// 	Description:	Obstacle map estimation: change maximum alowable error in range
	// 	Reference: 
	//  Input value:
	//  Return Value: 
	// --------------------------------------------------
	void MyForm::B_MAX_ERROR_RANGE_ValueChanged(System::Object^  sender, System::EventArgs^  e)
	{
		INITIAL_data->max_range_error_meter = System::Convert::ToInt32(B_MAX_ERROR_RANGE->Value);
	}
	// --------------------------------------------------
	//  B_WIRE_SEGMENT_LENGTH_METER_ValueChanged  
	//  Alon Slapak 24/4/2015
	// 	Description:	Typical length of wire between 2 pylons
	// 	Reference: 
	//  Input value:
	//  Return Value: 
	// --------------------------------------------------
	void MyForm::B_WIRE_SEGMENT_LENGTH_METER_ValueChanged(System::Object^  sender, System::EventArgs^  e)
	{
		INITIAL_data->wire_segment_length_meter = System::Convert::ToInt32(B_WIRE_SEGMENT_LENGTH_METER->Value);
	}
	// --------------------------------------------------
	//  convert_UTM_to_Screen  
	//  Alon Slapak 11/5/2015
	// 	Description:	Convert UTM to Screen coordinates
	// 	Reference:		Note, the problem is that the 0,0 is the top-left on screen
	//					and Y increases toward bottom edge
	//  Input value:	UTM coordinates
	//  Return Value:	Screen coordinates
	// --------------------------------------------------
	PointF MyForm::convert_UTM_to_Screen(PointD UTM_coordinates)
	{
		PointF			Screen_coordinates;
		float			Mega_meters_per_pixel = ((float)INITIAL_data->screen_width / 1000 / SCREEN_SIZE_X);
		//--------------------------------------
		//	Convert coordinates 
		//--------------------------------------
		Screen_coordinates.X = (float)(UTM_coordinates.X - INITIAL_data->GPS_longitude) / Mega_meters_per_pixel;
		Screen_coordinates.Y = (float)(UTM_coordinates.Y - INITIAL_data->GPS_latitude) / Mega_meters_per_pixel;
		//--------------------------------------
		//	Compensate for the opposite side of the (-600-->600, -500->500,...,0-->0) 
		//--------------------------------------
		Screen_coordinates.Y = -Screen_coordinates.Y;
		
		return Screen_coordinates;
	}
	// --------------------------------------------------
	//  convert_Screen_to_UTM  
	//  Alon Slapak 11/5/2015
	// 	Description:	Convert Screen to UTM coordinates
	// 	Reference:		Note, the problem is that the 0,0 is the top-left on screen
	//					and Y increases toward bottom edge
	//  Input value:	Sceren coordinates
	//  Return Value:	UTM coordinates
	// --------------------------------------------------
	PointD MyForm::convert_Screen_to_UTM(PointF Screen_coordinates)
	{
		PointD			UTM_coordinates;
		float			Mega_meters_per_pixel = ((float)INITIAL_data->screen_width / 1000 / SCREEN_SIZE_X);
		//--------------------------------------
		//	Convert coordinates 
		//--------------------------------------
		UTM_coordinates.X = Screen_coordinates.X * Mega_meters_per_pixel + INITIAL_data->GPS_longitude;
		UTM_coordinates.Y = -Screen_coordinates.Y * Mega_meters_per_pixel + INITIAL_data->GPS_latitude;
		
		return UTM_coordinates;
	}
	// --------------------------------------------------
	//  convert_Spherical_to_Cartesian  
	//  Alon Slapak 14/5/2015
	// 	Description:	convert_Spherical_to_Cartesian
	// 	Reference:		http://en.wikipedia.org/wiki/Spherical_coordinate_system
	//  Input value:	Spherical_point
	//  Return Value:	Cartesian_point
	// --------------------------------------------------
	PointD MyForm::convert_Spherical_to_Cartesian(PointD Spherical_point)
	{
		PointD			Cartesian_point;
		Cartesian_point.X = Spherical_point.X * cos(Spherical_point.Y);
		Cartesian_point.Y = Spherical_point.X * sin(Spherical_point.Y);
		return Cartesian_point;
	}
	// --------------------------------------------------
	//  convert_Cartesian_to_Spherical  
	//  Alon Slapak 14/5/2015
	// 	Description:	convert_Cartesian_to_Spherical
	// 	Reference:		http://en.wikipedia.org/wiki/Spherical_coordinate_system
	//  Input value:	Cartesian_point
	//  Return Value:	Spherical_point
	// --------------------------------------------------
	PointD MyForm::convert_Cartesian_to_Spherical(PointD Cartesian_point)
	{
		PointD			Spherical_point;
		Spherical_point.X = sqrt(pow(Cartesian_point.X, 2) + pow(Cartesian_point.Y, 2));
		Spherical_point.Y = atan2(Cartesian_point.X, Cartesian_point.Y);
		return Spherical_point;
	}
	// --------------------------------------------------
	//  Line_angle  
	//  Alon Slapak 13/2/2015
	// 	Description:	Calculate angle constructed by 3 points 
	// 	Reference:		http://math.stackexchange.com/questions/361412/finding-the-angle-between-three-points
	//  Input value:	Three points
	//  Return Value:	The angle p1 <- p2 -> p3 (namely, p2 is the vertex)
	// --------------------------------------------------
	double MyForm::Line_angle(PointD p1, PointD p2, PointD p3)
	{
		double x1, x2, y1, y2, t1, t2;
		double norm1, norm2, inner_prod;
		x1 = (p3.X - p2.X);
		y1 = (p3.Y - p2.Y);
		x2 = (p1.X - p2.X);
		y2 = (p1.Y - p2.Y);

		norm1 = sqrt(pow(x1, 2) + pow(y1, 2));
		norm2 = sqrt(pow(x2, 2) + pow(y2, 2));
		inner_prod = x1 * x2 + y1 * y2;
		t1 = inner_prod / (norm1*norm2);
		t2 = acos(t1);
		return (acos(inner_prod / (norm1*norm2)));
	}
	// --------------------------------------------------
	//  Distance_between_points  
	//  Alon Slapak 22/4/2015
	// 	Description:	Calculate distance between 2 points
	// 	Reference:		
	//  Input value:	2 points
	//  Return Value:	The distacne between the 2 points
	// --------------------------------------------------
	double MyForm::Distance_between_points(PointD p1, PointD p2)
	{
		return sqrt(pow(p1.X - p2.X, 2) + pow(p2.Y - p2.Y, 2));
	}
	// --------------------------------------------------
	//  Distance_between_point_and_line  
	//  Alon Slapak 24/4/2015
	// 	Description:	Calculate distance between point and line (given by two points)
	// 	Reference:		http://mathworld.wolfram.com/Point-LineDistance2-Dimensional.html Eq. (14)
	//  Input value:	point, Line (given by 2 points)
	//  Return Value:	The distacne between the point and the line
	// --------------------------------------------------
	double MyForm::Distance_between_point_and_line(PointD P, PointD L1, PointD L2)
	{
		double			denominator, numerator;
		numerator	= abs((L2.X-L1.X)*(L1.Y-P.Y) - (L1.X-P.X)*(L2.Y-L1.Y));
		denominator = Distance_between_points(L1,L2);
		return	numerator / denominator;
	}

	// --------------------------------------------------
	//  Line_point_projection  
	//  Alon Slapak 13/2/2015
	// 	Description:	Calculate the closest point on a line to external point 
	// 	Reference:		http://math.stackexchange.com/questions/13176/how-to-find-a-point-on-a-line-closest-to-another-given-point
	//  Input value:	Three points:
	//					The 1st is the external one
	//					The 2nd and 3rd define the line
	//  Return Value:	the closes point on the line
	// --------------------------------------------------
	PointD MyForm::Line_point_projection(PointD p1, PointD p2, PointD p3)
	{
		double t, x1, x2, y1, y2;
		PointD  closest_point;
		x1 = (p3.X - p2.X);
		y1 = (p3.Y - p2.Y);
		x2 = (p1.X - p2.X);
		y2 = (p1.Y - p2.Y);
		t = (x1 * x2 + y1 * y2) / (x1 * x1 + y1 * y1);
		closest_point.X = p2.X + t * (p3.X - p2.X);
		closest_point.Y = p2.Y + t * (p3.Y - p2.Y);

		return closest_point;
	}
	// --------------------------------------------------
	//  Target_is_in_RADAR_FOV  
	//  Alon Slapak 20/2/2015
	// 	Description:	Check if target can be seen by the radar. 
	// 	Reference:		
	//  Input value:	two points:
	//					Radar position
	//					Target position
	//  Return Value:	Good if target can bee seen, Fault else.
	// --------------------------------------------------
	int MyForm::Target_is_in_RADAR_FOV(PointD Radar, float Radar_angle, PointD Target)
	{
		double		target_angle;
		double		target_range;

		double		temp_range;
		// --------------------------------------------------
		//	Calculate range and angle to target
		// --------------------------------------------------
		target_range = sqrt(pow(Target.X - Radar.X, 2) + pow(Target.Y - Radar.Y, 2)) * 1e6;
		target_angle = atan2((Target.X - Radar.X), (Target.Y - Radar.Y)) * 180.0 / PI;

		temp_range = target_range * target_angle;
		cout << temp_range;
		// --------------------------------------------------
		//	Check if in FOV
		// --------------------------------------------------
		if ((target_range <= INITIAL_data->radar_range) && (fabs(target_angle - Radar_angle) <= INITIAL_data->radar_FOV / 2))
		{
			return GOOD;
		}
		else
		{
			return FAULT;
		}

	}
	// --------------------------------------------------
	//  Init_screen  
	//  Alon Slapak 10/4/2015
	// 	Description:	Clear screen and plot obstacles
	// 	Reference: 
	//  Input value:
	//  Return Value: 
	// --------------------------------------------------
	void MyForm::Init_screen()
	{
		Graphics^			Panel_graphics = B_PANEL->CreateGraphics();
		Panel_graphics->Clear(Color::Black);
		delete Panel_graphics;
		Obstacles_map_plot(OBSTACLES_map_true, COLOR_TRUE);
	}
	
	// --------------------------------------------------
	//  Init_flight_experiment  
	//  Alon Slapak 10/4/2015
	// 	Description:	Initialize flight experiment
	// 	Reference: 
	//  Input value:
	//  Return Value: 
	// --------------------------------------------------
	void MyForm::Init_flight_experiment()
	{
		// Added by Cogniteam
		helicopter_routing = gcnew System::Collections::Generic::List<PointD>();
		//---------------------------------------------------
		//		Prepare screen
		//---------------------------------------------------
		Init_screen();
		//---------------------------------------------------
		//		Init estimation error function
		//---------------------------------------------------
		Error_function_old_counter = 0;
		//---------------------------------------------------
		//		Initialize route for the helicopter
		//---------------------------------------------------
		if (INITIAL_data->route_new == true)
		{
			HELICOPTER_route_master.Helicopter_initial_angle = atan((float)((rand() % 100) - 50) / 1000) * 10;
			HELICOPTER_route_master.Helicopter_delta_angle = atan((float)((rand() % 100) - 50) / 5000);
			HELICOPTER_route_master.Helicopter_position = convert_Screen_to_UTM(PointF((float)SCREEN_SIZE_X / 2 + (rand() % 100) - 50, (float)SCREEN_SIZE_Y - 1));
			HELICOPTER_route_master.time_stamp = 0;
		}
		HELICOPTER_route = HELICOPTER_route_master;
		// Added by Cogniteam
		helicopter_routing->Add(HELICOPTER_route.Helicopter_position);
		//---------------------------------------------------
		//		Initialize estimated obstacle map
		//---------------------------------------------------
		OBSTACLES_map_estimated.number_of_obstacles = 0;
		//---------------------------------------------------
		//		Open file for serialization Targets
		//---------------------------------------------------
		Recording_file_handle = new fstream(TARGETS_FILE_NAME, ios::out | ios::binary);
		//---------------------------------------------------
		//		Avoid initializing again
		//---------------------------------------------------
		Flight_expeiment_initialized = true;
	}
	// --------------------------------------------------
	//  FInalize_flight_experiment  
	//  Alon Slapak 10/4/2015
	// 	Description:	Finalize flight experiment
	// 	Reference: 
	//  Input value:
	//  Return Value: 
	// --------------------------------------------------
	void MyForm::FInalize_flight_experiment()
	{
		//---------------------------------------------------
		//		Closure
		//---------------------------------------------------
		Recording_file_handle->close();
		delete Recording_file_handle;
		Flight_expeiment_initialized = false;
		// Added by Cogniteam
		GroundTruthToObj(helicopter_routing);
	}
	// --------------------------------------------------
	//  B_FLY_Click  
	//  Alon Slapak 13/2/2015
	// 	Description:	Fly and create the output files 
	// 	Reference: 
	//  Input value:
	//  Return Value: 
	// --------------------------------------------------
	void MyForm::B_FLY_Click(System::Object^  sender, System::EventArgs^  e)
	{
		int				i;
		//---------------------------------------------------
		//		Screen borders to UTM: Note, Y=0 at the top side and increases to the bottom side
		//---------------------------------------------------
		PointD			Min = convert_Screen_to_UTM(PointF(0, SCREEN_SIZE_Y));
		PointD			Max = convert_Screen_to_UTM(PointF(SCREEN_SIZE_X, 0));
		//---------------------------------------------------
		//		Init experiment (if not already initizalized)
		//---------------------------------------------------
		if (Flight_expeiment_initialized == false)
		{
			Init_flight_experiment();
		}
		//---------------------------------------------------
		//		Loop of the radar detection across the route
		//---------------------------------------------------
		while ((HELICOPTER_route.Helicopter_position.X > Min.X) && (HELICOPTER_route.Helicopter_position.X < Max.X) && (HELICOPTER_route.Helicopter_position.Y > Min.Y) && (HELICOPTER_route.Helicopter_position.Y < Max.Y))
		{
			Simulate_radar_operation();
			//---------------------------------------------------
			//		Move to next Point
			//---------------------------------------------------
			for (i = 0; i < (int)(INITIAL_data->Helicopter_Speed / INITIAL_data->radar_refresh_time); i++)
			{
				HELICOPTER_route.Helicopter_position.X += (double)(sin(HELICOPTER_route.Helicopter_angle * PI / 180) /1e6);
				HELICOPTER_route.Helicopter_position.Y += (double)(cos(HELICOPTER_route.Helicopter_angle * PI / 180) /1e6);
				HELICOPTER_route.Helicopter_angle += HELICOPTER_route.Helicopter_delta_angle;
			}
			HELICOPTER_route.time_stamp += (float)(1.0 / INITIAL_data->radar_refresh_time);
		}
		//---------------------------------------------------
		//		Closure
		//---------------------------------------------------
		FInalize_flight_experiment();
	}
	// --------------------------------------------------
	//  B_STEP_FORWARD_Click  
	//  Alon Slapak 10/4/2015
	// 	Description:	if "Fly step forward" was pressed, Move the helicopter one step forward to the 
	// 	Reference: 
	//  Input value:
	//  Return Value: 
	// --------------------------------------------------
	void MyForm::B_STEP_FORWARD_Click(System::Object^  sender, System::EventArgs^  e)
	{
		int				i;
		//---------------------------------------------------
		//		Screen borders to UTM: Note, Y=0 at the top side and increases to the bottom side
		//---------------------------------------------------
		PointD			Min = convert_Screen_to_UTM(PointF(0, SCREEN_SIZE_Y));
		PointD			Max = convert_Screen_to_UTM(PointF(SCREEN_SIZE_X, 0));
		//---------------------------------------------------
		//		Init experiment (if not already initizalized)
		//---------------------------------------------------
		if (Flight_expeiment_initialized == false)
		{
			Init_flight_experiment();
		}
		//---------------------------------------------------
		//		Loop of the radar detection across the route
		//---------------------------------------------------
		if ((HELICOPTER_route.Helicopter_position.X > Min.X) && (HELICOPTER_route.Helicopter_position.X < Max.X) && (HELICOPTER_route.Helicopter_position.Y > Min.Y) && (HELICOPTER_route.Helicopter_position.Y < Max.Y))
		{
			Simulate_radar_operation();
			//---------------------------------------------------
			//		Move to next Point
			//---------------------------------------------------
			for (i = 0; i < (int)(INITIAL_data->Helicopter_Speed / INITIAL_data->radar_refresh_time); i++)
			{
				HELICOPTER_route.Helicopter_position.X += (double)(sin(HELICOPTER_route.Helicopter_angle * PI / 180)) / 1e6;
				HELICOPTER_route.Helicopter_position.Y += (double)(cos(HELICOPTER_route.Helicopter_angle * PI / 180)) / 1e6;
				HELICOPTER_route.Helicopter_angle += HELICOPTER_route.Helicopter_delta_angle;
			}
			HELICOPTER_route.time_stamp += (float)(1.0 / INITIAL_data->radar_refresh_time);
		}
		else
		{
			//---------------------------------------------------
			//		Closure
			//---------------------------------------------------
			FInalize_flight_experiment();
			return;
		}	
	}
	// --------------------------------------------------
	//  Simulate_radar_operation  
	//  Alon Slapak 10/4/2015
	// 	Description:	Move the helicopter one step forward to the 
	// 	Reference: 
	//  Input value:
	//  Return Value: 
	// --------------------------------------------------
	void MyForm::Simulate_radar_operation()
	{
		int					i;
		float				False_alarm_draw;
		float				False_alarm_threshold;
		float				False_alarm_range;
		float				False_alarm_angle;
		PointD				target_UTM;
		PointD				noise_UTM;
		PointD				estimated_target_UTM;
		PointF				estimated_target_Screen;
		PointD				PNI;
		PointF				helicopter_Screen;
		T_Target			Target;
		Pen^				Pen_helicopter = gcnew Pen(Color::Red);
		Pen^				Pen_target_pylon = gcnew Pen(Color::Blue);
		Pen^				Pen_target_wire = gcnew Pen(Color::Pink);
		Pen^				Pen_target_false = gcnew Pen(Color::ForestGreen);
		Graphics^			panel_graphics = B_PANEL->CreateGraphics();
		//---------------------------------------------------
		//		Plot route
		//---------------------------------------------------	
		helicopter_Screen = convert_UTM_to_Screen(HELICOPTER_route.Helicopter_position);
		panel_graphics->DrawRectangle(Pen_helicopter, (int)helicopter_Screen.X - 1, (int)helicopter_Screen.Y - 1, 3, 3);
		//---------------------------------------------------
		//		Helicopter data --> target struct
		//---------------------------------------------------
		Target.time = HELICOPTER_route.time_stamp;								// [sec, 0 is a week start]
		Target.radar_lat = HELICOPTER_route.Helicopter_position.X;				// [Longitude coordiantes]
		Target.radar_lon = HELICOPTER_route.Helicopter_position.Y;				// [Latitude coordinates]
		Target.radar_alt = 100;													// [meters]
		Target.radar_att_roll;													// [deg, clockwise, 0 is North direction]
		Target.radar_att_pitch;													// [deg, clockwise, 0 is North direction]
		Target.radar_att_yaw;													// [deg, clockwise, 0 is North direction]
		Target.radar_ins_roll = 0;													// [rad, 0 is a start position]
		Target.radar_ins_pitch = 0;													// [rad, 0 is a start position]
		Target.radar_ins_yaw = HELICOPTER_route.Helicopter_angle;					// [rad, 0 is a start position]
		Target.radar_velocity_x = INITIAL_data->Helicopter_Speed * sin(HELICOPTER_route.Helicopter_angle * PI / 180);					// [m / sec, related to ? ? ? ? ]
		Target.radar_velocity_y = INITIAL_data->Helicopter_Speed * cos(HELICOPTER_route.Helicopter_angle * PI / 180);				// [m / sec, related to ? ? ? ? ]
		Target.radar_velocity_z = 0;													// [m / sec, related to ? ? ? ? ]
		//---------------------------------------------------
		//		Empty Radar_detections_array
		//---------------------------------------------------		
		for (i = 0; i < MAX_DETECTIONS; i++)
		{
			Radar_detections_array[i].target_reliability = -1;
		}
		Radar_detections_counter = 0;
		//---------------------------------------------------
		//		Detect Pylons
		//---------------------------------------------------
		for (i = 0; i < OBSTACLES_map_true.number_of_obstacles; i++)
		{
			if (OBSTACLES_map_true.Obstacles[i].Obstacle_type == OBSTACLE_PYLON)
			{
				//---------------------------------------------------
				//		Check if target is in radar FAV
				//---------------------------------------------------
				if (Target_is_in_RADAR_FOV(HELICOPTER_route.Helicopter_position, HELICOPTER_route.Helicopter_angle, OBSTACLES_map_true.Obstacles[i].Point_1) == GOOD)
				{
					//---------------------------------------------------
					//		Generate a radar detection
					//---------------------------------------------------
					noise_UTM.X = (float)((rand() % 100) - 50) / 50 * INITIAL_data->Variance_position_wires / 1e6;
					noise_UTM.Y = (float)((rand() % 100) - 50) / 50 * INITIAL_data->Variance_position_wires / 1e6;
					
					estimated_target_UTM.X = OBSTACLES_map_true.Obstacles[i].Point_1.X + noise_UTM.X;
					estimated_target_UTM.Y = OBSTACLES_map_true.Obstacles[i].Point_1.Y + noise_UTM.Y;
					//---------------------------------------------------
					//		Update detections array
					//---------------------------------------------------
					Target.target_reliability = 50;					// [0%:100%]
					PointD	Sperical_point = convert_Cartesian_to_Spherical(PointD((estimated_target_UTM.X - HELICOPTER_route.Helicopter_position.X) * 1e6, (estimated_target_UTM.Y - HELICOPTER_route.Helicopter_position.Y) * 1e6));
					Target.target_range = Sperical_point.X;					// [meters from radar]
					Target.target_azimuth = Sperical_point.Y;				// [rad, 0 is LOS, positive counterclockwise]
					Target.target_elevation = 0;							// [rad, 0 is LOS, positive counterclockwise]
					Target.target_doppler = 0;							// [m / sec]
					Target.target_polarization = PI / 2;				// [rad, counterclockwise, 0 is a horizontal]
					Radar_detections_array[Radar_detections_counter] = Target;
					Radar_detections_counter++;
					//---------------------------------------------------
					//		Debug: plot targets on screen
					//---------------------------------------------------
					if (INITIAL_data->show_detections == true)
					{
						estimated_target_Screen = convert_UTM_to_Screen(estimated_target_UTM);
						panel_graphics->DrawRectangle(Pen_target_pylon, (int)estimated_target_Screen.X - 6, (int)estimated_target_Screen.Y - 6, 13, 13);
					}
				}
			}
			//---------------------------------------------------
			//		Detect Wires
			//---------------------------------------------------
			if (OBSTACLES_map_true.Obstacles[i].Obstacle_type == OBSTACLE_WIRE)
			{
				//---------------------------------------------------
				//		Check if there is a PNI on the wire
				//---------------------------------------------------
				if ((Line_angle(HELICOPTER_route.Helicopter_position, OBSTACLES_map_true.Obstacles[i].Point_1, OBSTACLES_map_true.Obstacles[i].Point_2) < PI / 2) &&
					(Line_angle(HELICOPTER_route.Helicopter_position, OBSTACLES_map_true.Obstacles[i].Point_2, OBSTACLES_map_true.Obstacles[i].Point_1) < PI / 2))
				{
					//---------------------------------------------------
					//		Get the PNI position
					//---------------------------------------------------
					PNI = Line_point_projection(HELICOPTER_route.Helicopter_position, OBSTACLES_map_true.Obstacles[i].Point_1, OBSTACLES_map_true.Obstacles[i].Point_2);
					//---------------------------------------------------
					//		Check if target is in radar FAV
					//---------------------------------------------------
					if (Target_is_in_RADAR_FOV(HELICOPTER_route.Helicopter_position, HELICOPTER_route.Helicopter_angle, PNI) == GOOD)
					{
						//---------------------------------------------------
						//		Generate a radar detection
						//---------------------------------------------------
						noise_UTM.X = (float)((rand() % 100) - 50) / 50 * INITIAL_data->Variance_position_wires / 1e6;
						noise_UTM.Y = (float)((rand() % 100) - 50) / 50 * INITIAL_data->Variance_position_wires / 1e6;
						
						estimated_target_UTM.X = PNI.X + noise_UTM.X;
						estimated_target_UTM.Y = PNI.Y + noise_UTM.Y;
						//---------------------------------------------------
						//		Update detections array
						//---------------------------------------------------
						Target.target_reliability = 50;					// [0%:100%]
						PointD	Sperical_point = convert_Cartesian_to_Spherical(PointD((estimated_target_UTM.X - HELICOPTER_route.Helicopter_position.X) * 1e6, (estimated_target_UTM.Y - HELICOPTER_route.Helicopter_position.Y) * 1e6));
						Target.target_range = Sperical_point.X;					// [meters from radar]
						Target.target_azimuth = Sperical_point.Y;				// [rad, 0 is LOS, positive counterclockwise]
						Target.target_elevation = 0;							// [rad, 0 is LOS, positive counterclockwise]
						Target.target_doppler = 0;							// [m / sec]
						Target.target_polarization = 0;						// [rad, counterclockwise, 0 is a horizontal]
						Radar_detections_array[Radar_detections_counter] = Target;
						Radar_detections_counter++;
						//---------------------------------------------------
						//		Debug: plot targets on screen
						//---------------------------------------------------
						if (INITIAL_data->show_detections == true)
						{
							estimated_target_Screen = convert_UTM_to_Screen(estimated_target_UTM);
							panel_graphics->DrawRectangle(Pen_target_wire, (int)estimated_target_Screen.X - 6, (int)estimated_target_Screen.Y - 6, 13, 13);
						}
					}
				}
			}
		}
		//---------------------------------------------------
		//		False alarm
		//---------------------------------------------------
		False_alarm_draw = (float)(rand() % 10000) / 10000;			// U[0,1] distribution
		False_alarm_threshold = (float)(1.0 / (3600.0 / INITIAL_data->False_alarm_rate) / INITIAL_data->radar_refresh_time);
		if (False_alarm_draw < False_alarm_threshold)
		{
			//---------------------------------------------------
			//		Generate a radar detection
			//---------------------------------------------------
			False_alarm_range = (float)(rand() % INITIAL_data->radar_range);
			False_alarm_angle = (float)(rand() % INITIAL_data->radar_FOV) - INITIAL_data->radar_FOV / 2;
			False_alarm_angle += HELICOPTER_route.Helicopter_angle;

			estimated_target_UTM.X = HELICOPTER_route.Helicopter_position.X + (float)sin(False_alarm_angle / 180 * PI) * False_alarm_range / 1e6;
			estimated_target_UTM.Y = HELICOPTER_route.Helicopter_position.Y + (float)cos(False_alarm_angle / 180 * PI) * False_alarm_range / 1e6;
			//---------------------------------------------------
			//		Update detections array
			//---------------------------------------------------
			Target.target_reliability = 50;					// [0%:100%]
			PointD	Sperical_point = convert_Cartesian_to_Spherical(PointD((estimated_target_UTM.X - HELICOPTER_route.Helicopter_position.X) * 1e6, (estimated_target_UTM.Y - HELICOPTER_route.Helicopter_position.Y) * 1e6));
			Target.target_range = Sperical_point.X;					// [meters from radar]
			Target.target_azimuth = Sperical_point.Y;				// [rad, 0 is LOS, positive counterclockwise]
			Target.target_elevation = 0;							// [rad, 0 is LOS, positive counterclockwise]
			Target.target_doppler = 0;							// [m / sec]
			Target.target_polarization = PI / 2 * ((rand() % 2));				// [rad, counterclockwise, 0 is a horizontal]
			Radar_detections_array[Radar_detections_counter] = Target;
			Radar_detections_counter++;
			//---------------------------------------------------
			//		Debug: plot targets on screen
			//---------------------------------------------------
			if (INITIAL_data->show_detections == true)
			{
				estimated_target_Screen = convert_UTM_to_Screen(estimated_target_UTM);
				panel_graphics->DrawRectangle(Pen_target_false, (int)estimated_target_Screen.X - 6, (int)estimated_target_Screen.Y - 6, 13, 13);
			}
		}
		//---------------------------------------------------
		//		Save detections in file
		//---------------------------------------------------
		for (i = 0; i < Radar_detections_counter; i++)
		{
			pin_ptr<T_Target> pinnedPtr = &Radar_detections_array[i];
			Recording_file_handle->write((char*)(pinnedPtr), sizeof(T_Target));
		}
		//---------------------------------------------------
		//		Clean up
		//---------------------------------------------------
		delete panel_graphics;
		//---------------------------------------------------
		//		Estimate the obstacles map
		//---------------------------------------------------
		Obstacles_map_estimate();
		Obstacles_map_plot(OBSTACLES_map_estimated, COLOR_ESTIMATED);
		Obstacles_map_error_function();
	}
	// --------------------------------------------------
	//  Obstacles_map_estimate  
	//  Alon Slapak 10/4/2015
	// 	Description:	Estimate the obstacles map
	// 	Reference: 
	//  Input value:
	//  Return Value: 
	// --------------------------------------------------
	void MyForm::Obstacles_map_estimate()
	{
		int				i, k;
		bool			flag_pylon_detected;
		bool			flag_wire_detected;
		PointD			target;
		double			theta;
		double			delta_X, delta_Y;


		for (i = 0; i < Radar_detections_counter; i++)
		{
			//-----------------------------------------
			//		Pylons
			//-----------------------------------------
			if (Radar_detections_array[i].target_polarization == PI / 2)
			{
				flag_pylon_detected = false;
				for (k = 0; k < OBSTACLES_map_estimated.number_of_obstacles; k++)
				{
					if (OBSTACLES_map_estimated.Obstacles[i].Obstacle_type == OBSTACLE_PYLON)
					{
						//-----------------------------------------
						//		Reinforce detected pylon  
						//-----------------------------------------
						PointD	Cartesian_point = convert_Spherical_to_Cartesian(PointD(Radar_detections_array[i].target_range, Radar_detections_array[i].target_azimuth));
						target.X = HELICOPTER_route.Helicopter_position.X + Cartesian_point.Y / 1e6;  // X of helicopter is horizontal (Screen), and X target is vertical? 
						target.Y = HELICOPTER_route.Helicopter_position.Y + Cartesian_point.X / 1e6;  // Todo alon 14.5.2015

						if (Distance_between_points(target, OBSTACLES_map_estimated.Obstacles[k].Point_1) < INITIAL_data->max_range_error_meter)
						{
							flag_pylon_detected = true;
							OBSTACLES_map_estimated.Obstacles[k].Obstacle_reliability = OBSTACLES_map_estimated.Obstacles[k].Obstacle_reliability / 100 * INITIAL_data->reliability_threshold + (100 - (double)INITIAL_data->reliability_threshold);
						}
					}
				}
				//-----------------------------------------
				//		Add new pylon that was detected 
				//-----------------------------------------
				if ((flag_pylon_detected == false) && (OBSTACLES_map_estimated.number_of_obstacles < MAX_OBSTACLES))
				{
					PointD	Cartesian_point = convert_Spherical_to_Cartesian(PointD(Radar_detections_array[i].target_range, Radar_detections_array[i].target_azimuth));
					target.X = HELICOPTER_route.Helicopter_position.X + Cartesian_point.Y / 1e6;  // X of helicopter is horizontal (Screen), and X target is vertical? 
					target.Y = HELICOPTER_route.Helicopter_position.Y + Cartesian_point.X / 1e6;  // Todo alon 14.5.2015
					OBSTACLES_map_estimated.Obstacles[OBSTACLES_map_estimated.number_of_obstacles].Obstacle_type = OBSTACLE_PYLON;
					OBSTACLES_map_estimated.Obstacles[OBSTACLES_map_estimated.number_of_obstacles].Point_1 = target;
					OBSTACLES_map_estimated.Obstacles[OBSTACLES_map_estimated.number_of_obstacles].Obstacle_reliability = (100 - INITIAL_data->reliability_threshold);
					OBSTACLES_map_estimated.number_of_obstacles++;
				}
			}
			//-----------------------------------------.
			//		Wires  
			//-----------------------------------------
			else
			{
				flag_wire_detected = false;
				for (k = 0; k < OBSTACLES_map_estimated.number_of_obstacles; k++)
				{
					if (OBSTACLES_map_estimated.Obstacles[i].Obstacle_type == OBSTACLE_WIRE)
					{
						//-----------------------------------------
						//		Reinforce detected pylon  
						//-----------------------------------------
						PointD	Cartesian_point = convert_Spherical_to_Cartesian(PointD(Radar_detections_array[i].target_range, Radar_detections_array[i].target_azimuth));
						target.X = HELICOPTER_route.Helicopter_position.X + Cartesian_point.Y / 1e6;  // X of helicopter is horizontal (Screen), and X target is vertical? 
						target.Y = HELICOPTER_route.Helicopter_position.Y + Cartesian_point.X / 1e6;  // Todo alon 14.5.2015

						if (Distance_between_point_and_line(target, OBSTACLES_map_estimated.Obstacles[k].Point_1, OBSTACLES_map_estimated.Obstacles[k].Point_2)< INITIAL_data->max_range_error_meter)
						{
							flag_wire_detected = true;
							OBSTACLES_map_estimated.Obstacles[k].Obstacle_reliability = OBSTACLES_map_estimated.Obstacles[k].Obstacle_reliability / 100 * INITIAL_data->reliability_threshold + (100 - (double)INITIAL_data->reliability_threshold);
						}
					}
				}
				//-----------------------------------------
				//		Add new pylon that was detected 
				//-----------------------------------------
				if ((flag_wire_detected == false) && (OBSTACLES_map_estimated.number_of_obstacles < MAX_OBSTACLES))
				{
					PointD	Cartesian_point = convert_Spherical_to_Cartesian(PointD(Radar_detections_array[i].target_range, Radar_detections_array[i].target_azimuth));
					target.X = HELICOPTER_route.Helicopter_position.X + Cartesian_point.Y / 1e6;  // X of helicopter is horizontal (Screen), and X target is vertical? 
					target.Y = HELICOPTER_route.Helicopter_position.Y + Cartesian_point.X / 1e6;  // Todo alon 14.5.2015
					OBSTACLES_map_estimated.Obstacles[OBSTACLES_map_estimated.number_of_obstacles].Obstacle_type = OBSTACLE_WIRE;
					OBSTACLES_map_estimated.Obstacles[OBSTACLES_map_estimated.number_of_obstacles].Point_1 = target;
					//-----------------------------------------
					//		Find the bearing angle to the target
					//-----------------------------------------
					theta = atan2(target.X - HELICOPTER_route.Helicopter_position.X, -target.Y + HELICOPTER_route.Helicopter_position.Y);
					//-----------------------------------------
					//		Find Point_1 & Point_2
					//-----------------------------------------
					delta_X = (float)(cos(theta) * INITIAL_data->wire_segment_length_meter / 2) / 1e6;
					delta_Y = (float)(sin(theta) * INITIAL_data->wire_segment_length_meter / 2) / 1e6;
					OBSTACLES_map_estimated.Obstacles[OBSTACLES_map_estimated.number_of_obstacles].Point_1.X = target.X + delta_X;
					OBSTACLES_map_estimated.Obstacles[OBSTACLES_map_estimated.number_of_obstacles].Point_1.Y = target.Y + delta_Y;
					OBSTACLES_map_estimated.Obstacles[OBSTACLES_map_estimated.number_of_obstacles].Point_2.X = target.X - delta_X;
					OBSTACLES_map_estimated.Obstacles[OBSTACLES_map_estimated.number_of_obstacles].Point_2.Y = target.Y - delta_Y;
					//-----------------------------------------
					//		finde the closest pylons
					//-----------------------------------------
				/*	for (k = 0; k < OBSTACLES_map_estimated.number_of_obstacles; k++)
					{
						if (OBSTACLES_map_estimated.Obstacles[k].Obstacle_type == OBSTACLE_PYLON)
						{
							if (Distance_between_points(OBSTACLES_map_estimated.Obstacles[OBSTACLES_map_estimated.number_of_obstacles].Point_1, OBSTACLES_map_estimated.Obstacles[k].Point_1) * meters_per_pixel < INITIAL_data->max_range_error_meter)
							{
								OBSTACLES_map_estimated.Obstacles[OBSTACLES_map_estimated.number_of_obstacles].Point_1 = OBSTACLES_map_estimated.Obstacles[k].Point_1;
							}
							if (Distance_between_points(OBSTACLES_map_estimated.Obstacles[OBSTACLES_map_estimated.number_of_obstacles].Point_2, OBSTACLES_map_estimated.Obstacles[k].Point_1) * meters_per_pixel < INITIAL_data->max_range_error_meter)
							{
								OBSTACLES_map_estimated.Obstacles[OBSTACLES_map_estimated.number_of_obstacles].Point_2 = OBSTACLES_map_estimated.Obstacles[k].Point_1;
							}
						}
					}*/

					//-----------------------------------------
					//		Reliability
					//-----------------------------------------
					OBSTACLES_map_estimated.Obstacles[OBSTACLES_map_estimated.number_of_obstacles].Obstacle_reliability = (100 - INITIAL_data->reliability_threshold);
					OBSTACLES_map_estimated.number_of_obstacles++;
				}
			}
		}
	}
	// --------------------------------------------------
	//  Obstacles_map_error_function  
	//  Alon Slapak 21/4/2015
	// 	Description:	Calculate the error function
	// 	Reference: 
	//  Input value:
	//  Return Value: 
	// --------------------------------------------------
	void MyForm::Obstacles_map_error_function()
	{
		double				estimation_error_FA;
		double				estimation_error_PD;
		int					i, m;
		double				range_error;
		double				meters_per_pixel = ((float)INITIAL_data->screen_width * 1000 / SCREEN_SIZE_X);
		double				minimum_range;
		int					estimation_error_counter;
		int					estimation_error_index;


		double				D_max = 50;
		double				RISK_PD = 100;
		double				RISK_FA = 50;

		// --------------------------------------------------
		//	Calculate error function: FA part
		// --------------------------------------------------
		estimation_error_FA = 0;
		estimation_error_counter = 0;
		for (i = 0; i < OBSTACLES_map_estimated.number_of_obstacles; i++)
		{
			if (OBSTACLES_map_estimated.Obstacles[i].Obstacle_reliability > INITIAL_data->reliability_threshold)
			{
				// --------------------------------------------------
				//	Pylon
				// --------------------------------------------------
				if (OBSTACLES_map_estimated.Obstacles[i].Obstacle_type == OBSTACLE_PYLON)
				{
					// --------------------------------------------------
					//	Find minimum distance from all true pylons
					// --------------------------------------------------
					minimum_range = 10000000;
				
					for (m = 0; m < OBSTACLES_map_true.number_of_obstacles; m++)
					{
						if (OBSTACLES_map_true.Obstacles[m].Obstacle_type == OBSTACLE_PYLON)
						{
							range_error = Distance_between_points(OBSTACLES_map_true.Obstacles[m].Point_1, OBSTACLES_map_estimated.Obstacles[i].Point_1);
							if (range_error < minimum_range)
							{
								minimum_range = range_error;
							}
						}
					}
				}
				// --------------------------------------------------
				//	Update estimation_error
				// --------------------------------------------------
				if (minimum_range != 10000000)
				{
					minimum_range = minimum_range / D_max;
					if (minimum_range > 1)
					{
						minimum_range = 1;
					}
					estimation_error_FA += minimum_range;
					estimation_error_counter++;
				}
			}
		}
		if (estimation_error_counter == 0)
		{
			estimation_error_FA = 0;
		}
		else
		{
			estimation_error_FA = estimation_error_FA / estimation_error_counter;
		}	
		// --------------------------------------------------
		//	Calculate error function: PD part
		// --------------------------------------------------
		estimation_error_PD = 0;
		estimation_error_counter = 0;
		for (i = 0; i < OBSTACLES_map_true.number_of_obstacles; i++)
		{
			// --------------------------------------------------
			//	Pylon
			// --------------------------------------------------
			if (OBSTACLES_map_true.Obstacles[i].Obstacle_type == OBSTACLE_PYLON)
			{
				// --------------------------------------------------
				//	Find minimum distance from all true pylons
				// --------------------------------------------------
				minimum_range = 10000000;
				for (m = 0; m < OBSTACLES_map_estimated.number_of_obstacles; m++)
				{
					if ((OBSTACLES_map_estimated.Obstacles[m].Obstacle_type == OBSTACLE_PYLON) && (OBSTACLES_map_estimated.Obstacles[m].Obstacle_reliability > INITIAL_data->reliability_threshold))
					{
						range_error = Distance_between_points(OBSTACLES_map_true.Obstacles[m].Point_1, OBSTACLES_map_estimated.Obstacles[i].Point_1);
						if (range_error < minimum_range)
						{
							minimum_range = range_error;
						}
					}
				}
				// --------------------------------------------------
				//	Update estimation_error
				// --------------------------------------------------
				if (minimum_range < D_max)
				{
					minimum_range = 0;
				}
				else
				{
					minimum_range = 1;
				}
				estimation_error_PD += minimum_range;
				estimation_error_counter++;
			}
		}
		if (estimation_error_counter == 0)
		{
			estimation_error_PD = 0;
		}
		else
		{
			estimation_error_PD = estimation_error_PD / estimation_error_counter;
		}
		// --------------------------------------------------
		//	Normalization
		// --------------------------------------------------
		estimation_error_index = (int)(SCREEN_SIZE_Y - HELICOPTER_route.Helicopter_position.Y);
		for (i = Error_function_old_counter; i < estimation_error_index; i++)
		{
			Error_function_array[i + 1] = (estimation_error_PD * RISK_PD + estimation_error_FA * RISK_FA) / (RISK_PD + RISK_FA);
			Error_function_old_counter = estimation_error_index;
		}
		
		// --------------------------------------------------
		//	Update error graph
		// --------------------------------------------------
		B_ERROR_FUNCTION->Series["error_function"]->Points->Clear();
		for (i = 0; i < estimation_error_index; i++)
		{
			B_ERROR_FUNCTION->Series["error_function"]->Points->AddXY(i, Error_function_array[i]);
		}
		B_ERROR_FUNCTION->Refresh();
	}

	// --------------------------------------------------
	//  B_SAVE_DETECTIONS_FILE_Click  
	//  Alon Slapak 2/6/2015
	// 	Description:	Save detections file 
	// 	Reference: 
	//  Input value:
	//  Return Value: 
	// --------------------------------------------------
	void MyForm::B_SAVE_DETECTIONS_FILE_Click(System::Object^  sender, System::EventArgs^  e)
	{

	}
	// --------------------------------------------------
	//  B_SAVE_OBSTACLES_FILE_Click  
	//  Alon Slapak 10/4/2015
	// 	Description:	Save obstacles file 
	// 	Reference: 
	//  Input value:
	//  Return Value: 
	// --------------------------------------------------
	void MyForm::B_SAVE_OBSTACLES_FILE_Click(System::Object^  sender, System::EventArgs^  e)
	{
		fstream		file_stream;
		int			i;

		B_SAVE_OBSTACLES_FILE_DIALOGE->DefaultExt = "txt";
		B_SAVE_OBSTACLES_FILE_DIALOGE->FileName = B_SAVE_OBSTACLES_FILE_DIALOGE->FileName;
		B_SAVE_OBSTACLES_FILE_DIALOGE->ShowDialog();
		B_OBSTACLES_file->Text = B_SAVE_OBSTACLES_FILE_DIALOGE->FileName;
		marshal_context ^ context = gcnew marshal_context();
		sprintf_s(INITIAL_data->Obstackes_file_name, context->marshal_as<const char*>(B_SAVE_OBSTACLES_FILE_DIALOGE->FileName));
		// --------------------------------------------------
		//			save file
		// --------------------------------------------------
		file_stream.open(INITIAL_data->Obstackes_file_name, std::ios_base::out);
		if (file_stream.is_open())
		{
			file_stream << OBSTACLES_map_true.number_of_obstacles << '\n';
			for (i = 0; i < OBSTACLES_map_true.number_of_obstacles; i++)
			{
				file_stream << OBSTACLES_map_true.Obstacles[i].Obstacle_type << '\t';
				file_stream << fixed << setprecision(6) << OBSTACLES_map_true.Obstacles[i].Point_1.Y << '\t' << fixed << setprecision(6) << OBSTACLES_map_true.Obstacles[i].Point_1.X << '\t';
				if (OBSTACLES_map_true.Obstacles[i].Obstacle_type == OBSTACLE_PYLON)
				{
					file_stream << "00.000000" << '\t' << "00.000000" << '\t';
				}
				else
				{
					file_stream << fixed << setprecision(6) << OBSTACLES_map_true.Obstacles[i].Point_2.Y << '\t' << fixed << setprecision(6) << OBSTACLES_map_true.Obstacles[i].Point_2.X << '\t';
				}
				file_stream << fixed << setprecision(0) << OBSTACLES_map_true.Obstacles[i].Obstacle_reliability << '\n';
			}
			file_stream.close();
		}
	}
	// --------------------------------------------------
	//  B_BROWSE_OBSTACLES_FILE_Click  
	//  Alon Slapak 10/4/2015
	// 	Description:	Load obstacles file  
	// 	Reference: 
	//  Input value:
	//  Return Value: 
	// --------------------------------------------------
	void MyForm::B_BROWSE_OBSTACLES_FILE_Click(System::Object^  sender, System::EventArgs^  e)
	{
		// --------------------------------------------------
		//			Clear screen
		// --------------------------------------------------
		OBSTACLES_map_true.number_of_obstacles = 0;
		B_PANEL->Invalidate();
		// --------------------------------------------------
		//			ask for file name
		// --------------------------------------------------
		B_OPEN_OBSTACLES_FILE->FileName = B_OBSTACLES_file->Text;
		B_OPEN_OBSTACLES_FILE->ShowDialog();
		B_OBSTACLES_file->Text = B_OPEN_OBSTACLES_FILE->FileName;
		marshal_context ^ context = gcnew marshal_context();
		sprintf_s(INITIAL_data->Obstackes_file_name, context->marshal_as<const char*>(B_OPEN_OBSTACLES_FILE->FileName));
		// --------------------------------------------------
		//			Load file
		// --------------------------------------------------
		Obstacles_map_load(OBSTACLES_map_true, INITIAL_data->Obstackes_file_name);
		// --------------------------------------------------
		//			Plot data
		// --------------------------------------------------
		Obstacles_map_plot(OBSTACLES_map_true, COLOR_TRUE);
	}
	// --------------------------------------------------
	//  Obstacles_map_load  
	//  Alon Slapak 13/5/2015
	// 	Description:	Load obstacles file  
	// 	Reference: 
	//  Input value:
	//  Return Value: 
	// --------------------------------------------------
	void MyForm::Obstacles_map_load(T_OBSTACLES_map % obstacles_map, char* file_name)
	{
		std::ifstream		file_stream;
		int					i;
		int					temp_int;
		float				temp_float_1, temp_float_2;

		file_stream.open(file_name, std::ios_base::in);
		if (file_stream.is_open())
		{
			file_stream >> temp_int;
			obstacles_map.number_of_obstacles = temp_int;
			for (i = 0; i < obstacles_map.number_of_obstacles; i++)
			{
				file_stream >> temp_int;
				obstacles_map.Obstacles[i].Obstacle_type = temp_int;
				file_stream >> temp_float_1 >> temp_float_2;
				obstacles_map.Obstacles[i].Point_1.Y = temp_float_1;
				obstacles_map.Obstacles[i].Point_1.X = temp_float_2;
				file_stream >> temp_float_1 >> temp_float_2;
				obstacles_map.Obstacles[i].Point_2.Y = temp_float_1;
				obstacles_map.Obstacles[i].Point_2.X = temp_float_2;
				file_stream >> temp_float_1;
				obstacles_map.Obstacles[i].Obstacle_reliability = temp_float_1;
			}
			file_stream.close();
		}
	}
	// --------------------------------------------------
	//  B_LOAD_DETCTIONS_Click  
	//  Alon Slapak 12/5/2015
	// 	Description:	Load Detections file  
	// 	Reference: 
	//  Input value:
	//  Return Value: 
	// --------------------------------------------------
	void MyForm::B_LOAD_DETCTIONS_Click(System::Object^  sender, System::EventArgs^  e)
	{
		T_Target			detection;
		Pen^				Pen_helicopter = gcnew Pen(Color::Red);
		Pen^				Pen_target_pylon = gcnew Pen(Color::Blue);
		Pen^				Pen_target_wire = gcnew Pen(Color::Pink);
		Graphics^			panel_graphics = B_PANEL->CreateGraphics();
		PointF				helicopter_Screen;
		PointD				detection_Screen;
		PointD				Min = convert_Screen_to_UTM(PointF(0, SCREEN_SIZE_Y));
		PointD				Max = convert_Screen_to_UTM(PointF(SCREEN_SIZE_X, 0));
		//---------------------------------------------------
		//		Open file for serialization Targets
		//---------------------------------------------------
		Recording_file_handle = new fstream(TARGETS_FILE_NAME, ios::in | ios::binary);
		//---------------------------------------------------
		//		Loop of the radar detection across the route
		//---------------------------------------------------
		while (!Recording_file_handle->eof())
		{
			//---------------------------------------------------
			//		Read target
			//---------------------------------------------------
			pin_ptr<T_Target> pinnedPtr = &detection;
			Recording_file_handle->read((char*)(pinnedPtr),sizeof(T_Target));
			//---------------------------------------------------
			//		Calculate helicopter position
			//---------------------------------------------------	
			helicopter_Screen = convert_UTM_to_Screen(PointD(detection.radar_lon - INITIAL_data->GPS_longitude, detection.radar_lat - INITIAL_data->GPS_latitude));
			if ((helicopter_Screen.X > Min.X) && (helicopter_Screen.X < Max.X) && (helicopter_Screen.Y > Min.Y) && (helicopter_Screen.Y < Max.Y))
			{
				//---------------------------------------------------
				//		Plot helicopter
				//---------------------------------------------------	
				panel_graphics->DrawRectangle(Pen_helicopter, (int)helicopter_Screen.X - 1, (int)helicopter_Screen.Y - 1, 3, 3);
				//---------------------------------------------------
				//		Calculate target position
				//---------------------------------------------------	
				detection_Screen = convert_Spherical_to_Cartesian(PointD(detection.target_range, detection.target_azimuth));
				detection_Screen.X += helicopter_Screen.X;
				detection_Screen.Y += helicopter_Screen.Y;
				//---------------------------------------------------
				//		Plot target
				//---------------------------------------------------	
				if (abs(detection.target_polarization) < PI / 4)
				{
					panel_graphics->DrawRectangle(Pen_target_wire, (int)detection_Screen.X - 6, (int)detection_Screen.Y - 6, 13, 13);
				}
				else
				{
					panel_graphics->DrawRectangle(Pen_target_pylon, (int)detection_Screen.X - 6, (int)detection_Screen.Y - 6, 13, 13);
				}
			}
		}
		//---------------------------------------------------
		//		Closure
		//---------------------------------------------------
		Recording_file_handle->close();
		delete Recording_file_handle;
		delete panel_graphics;
		return;
	}
	// --------------------------------------------------
	//  B_LOAD_MAP_IMAGE_Click  
	//  Alon Slapak 12/5/2015
	// 	Description:	Load MAP_IMAGE file  
	// 	Reference: 
	//  Input value:
	//  Return Value: 
	// --------------------------------------------------
	void MyForm::B_LOAD_MAP_IMAGE_Click(System::Object^  sender, System::EventArgs^  e)
	{

	}

	// --------------------------------------------------
	//  GroundTruthToObj  
	//  Ofir Iluz		19.3.2015 (Cogniteam)
	// 	Description:	receives the path of the helicopter(List<PointF>) which is another addition on the fly method
	//					additinaly, a new object's file will be created from this method 
	//					(name is defined in the h file), there is an example of that file on the attached files
	// 	Reference: 
	//  Input value:
	//  Return Value: 
	// --------------------------------------------------
	void MyForm::GroundTruthToObj(System::Collections::Generic::List<PointD>^ helicopter_route)
	{
		static const int STARTING_POINT = 3;
		static const int AMOUNT = 3;
		static const int LINE_TYPE = 2;
		static const int POINT_TYPE = 1;
		static const int LLA = 2;
		static const int NED = 1;
		static const int PYLON = 10;
		static const int POWERLINE = 11;
		static const int HELICOPTER = 12;
		static const int NED_PYLON = 20;
		static const int NED_POWERLINE = 21;
		static const int NED_HELICOPTER = 22;

		double meters_per_pixel = ((float)INITIAL_data->screen_width * 1000 / SCREEN_SIZE_X);
		double helicopter_pos_X = HELICOPTER_route.Helicopter_position.X;
		double helicopter_pos_Y = HELICOPTER_route.Helicopter_position.Y;
		double helicopter_radar__lat = helicopter_pos_X * meters_per_pixel;
		double helicopter_radar__lon = helicopter_pos_Y * meters_per_pixel;
		fstream out = fstream(GROUND_TRUTHS_FILE_NAME, ios::out);
		out << STARTING_POINT << std::endl;
		/*out << OBSTACLES_map_true.Obstacles[0] * 2 + helicopter_route->Count + 7 << std::endl;
		out << AMOUNT << std::endl;
		out << LINE_TYPE << " " << OBSTACLES_map_true.Obstacles[0].number_of_points << std::endl;
		for (int p = 0; p < OBSTACLES_map_true.; p++)
		{
			out << ((OBSTACLES_map_true.Obstacles[0].Point_1.X - helicopter_pos_X) * meters_per_pixel) + helicopter_radar__lat << " " << -((helicopter_pos_Y - OBSTACLES_map_true.Obstacles[0].Array_points[p].Y) * meters_per_pixel) + helicopter_radar__lon << " " << 0 << " " << NED_POWERLINE << std::endl;
		}
		out << LINE_TYPE << " " << helicopter_route->Count << std::endl;
		for (int p = 0; p < helicopter_route->Count; p++)
			out << ((helicopter_route[p].X - helicopter_pos_X)*meters_per_pixel) + helicopter_radar__lat << " " << -((helicopter_pos_Y - helicopter_route[p].Y)*meters_per_pixel) + helicopter_radar__lon << " " << 0 << " " << NED_HELICOPTER << std::endl;
		out << POINT_TYPE << " " << OBSTACLES_map_true.Obstacles[0].number_of_points << std::endl;
		for (int p = 0; p < OBSTACLES_map_true.Obstacles[0].number_of_points; p++)
			out << ((helicopter_route[p].X - helicopter_pos_X) * meters_per_pixel) + helicopter_radar__lat << " " << -((helicopter_pos_Y - OBSTACLES_map_true.Obstacles[0].Array_points[p].Y) * meters_per_pixel) + helicopter_radar__lon << " " << 0 << " " << NED_PYLON << std::endl;
		out << 8 << std::endl;*/
		out << NED << " " << 1 << " " << "ned" << std::endl;
		out << LLA << " " << 1 << " " << "lla" << std::endl;
		out << PYLON << " " << 2 << " " << "pylon" << std::endl;
		out << POWERLINE << " " << 2 << " " << "powerline" << std::endl;
		out << HELICOPTER << " " << 2 << " " << "helicopter" << std::endl;
		out << NED_PYLON << " " << 3 << " " << NED << " " << PYLON << std::endl;
		out << NED_POWERLINE << " " << 3 << " " << NED << " " << POWERLINE << std::endl;
		out << NED_HELICOPTER << " " << 3 << " " << NED << " " << HELICOPTER << std::endl;
		out.close();
	}
}