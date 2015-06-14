#pragma once


#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <istream>
#include <string>
#include <vector>
#include <msclr/marshal.h>
#include <msclr/marshal_cppstd.h>
#include <iomanip>


//-----------------------------------------
//		Misc.
//-----------------------------------------
#define	GOOD							0
#define FAULT							1
#define MAX_STRING_LENGTH				512
//-----------------------------------------
//		FILES & FOLDERS
//-----------------------------------------
#define EXPERIMENT_NUMBER_FILE			"expNum.txt"
#define	EXPERIMENT_DEF_FILE				"LOG.txt"
#define	EXPERIMENT_DETECTIONS_FILE		"Detections.bin"
#define	INITIAL_VALUES_FILE_NAME		"INIT_FORM.bin"
#define GROUND_TRUTHS_FILE_NAME			"ground_truths.obj"
//-----------------------------------------
//		Display
//-----------------------------------------
#define SCREEN_SIZE_X					600
#define SCREEN_SIZE_Y					600
#define MAP_BASE_LAT					32.056143
#define MAP_BASE_LON					34.931614

#define	OBSTACLE_PYLON					1
#define	OBSTACLE_WIRE					2
#define PYLON_SIZE_TRUE					5
#define PYLON_SIZE_ESTIMATED			11
#define COLOR_TRUE						Color::White
#define COLOR_ESTIMATED					Color::Yellow

//-----------------------------------------
//		Constants
//-----------------------------------------

//-----------------------------------------
//		Obstacles map
//-----------------------------------------
#define	MAX_POINTS_IN_OBSTACLE			100
#define	MAX_OBSTACLES					100
#define	MAX_DETECTIONS					100
#define MAX_RANGE_ERROR					50	   //[m]
#define RELIABILITY_THRESHOLD			70
#define WIRE_SEGMENT_LENGTH_METER		400

#define	PI								3.14159265

//-----------------------------------------
//		Struct:		T_INITIAL_data
//-----------------------------------------
typedef struct T_INITIAL_data
{
	int							Helicopter_Speed;
	int							Variance_position_wires;
	int							False_alarm_rate;
	int							radar_range;
	int							radar_FOV;
	int							radar_refresh_time;
	int							screen_width;
	bool						route_new;
	bool						show_detections;
	char						OBSTACLES_file_name[MAX_STRING_LENGTH];
	char						EXPERIMENT_directory[MAX_STRING_LENGTH];
	char						DETECTIONS_folder[MAX_STRING_LENGTH];
	char						MAP_image_file[MAX_STRING_LENGTH];
	int							max_range_error_meter;
	int							reliability_threshold;
	int							wire_segment_length_meter;
	double						GPS_latitude;
	double						GPS_longitude;
} T_INITIAL_data;


//-----------------------------------------
//		namespace ROD_OMR_V1 
//-----------------------------------------
namespace ROD_OMR_V1
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace msclr::interop;
	using namespace std;
	using	std::fstream;
	using	std::ios;
	using	std::string;

	//-----------------------------------------
	//		Struct:		T_Aux_data
	//  Todo Alon, 6.8.2013: (1) change to 3d, (2) add =, +,-, and scalar multiplication methods 
	//		(3) add transformation from cartesian to spherical, to UTM, to NED and to screen.
	//-----------------------------------------
	public value struct PointD
	{
		PointD(double x, double y){ X = x; Y = y; }
		double	X;
		double	Y;
	};

	//-----------------------------------------
	//		Struct:		T_Aux_data
	//-----------------------------------------
	public value struct T_Aux_data
	{
		double					obstacle_reliability;
	};
	//-----------------------------------------
	//		Struct:		T_obstacle
	//-----------------------------------------
	public value struct T_Obstacle
	{
		int						Obstacle_type;
		PointD					Point_1;
		PointD					Point_2;
		double					Obstacle_reliability;
	};
	//-----------------------------------------
	//		Struct:		T_OBSTACLES_map
	//-----------------------------------------
	public value struct T_OBSTACLES_map
	{
		int						number_of_obstacles;
		array<T_Obstacle>^		Obstacles;
	};
	//-----------------------------------------
	//		Struct:		T_HELICOPTER_route
	//-----------------------------------------
	public value struct T_HELICOPTER_route
	{
		float				Helicopter_initial_YAW;
		float				Helicopter_delta_YAW;
		float				Helicopter_YAW;
		PointD				Helicopter_UTM;
		float				time_stamp;
	};
	// ---------------------------------------- -
	//		SENSOR
	//-----------------------------------------
	public value struct T_SENSOR_data
	{
		//ISL								//Unit			Description
		double 			ROW_YAW;			//deg			Yaw angle relative to true north
		double 			ROW_PITCH;			//deg			Pitch angle relative to horizon
		double 			ROW_ROLL;			//deg			Pool angle relative to horizon
		double 			Latitude;			//deg			Estimated position in geodetic latitude
		double 			Longitude;			//deg			Estimated position in geodetic longitude
		double 			Altitude;			//m				Estimated height above ellipsoid. (WGS84)
		double 			VelocityX;			//m/s			Estimated velocity in NED frame. (North)
		double 			VelocityY;			//m/s			Estimated velocity in NED frame. (East)
		double 			VelocityZ;			//m/s			Estimated velocity in NED frame. (Down)
		double 			AccelX;				//m/s^2			Estimated acceleration in body frame. (X-axis)
		double 			AccelY;				//m/s^2			Estimated acceleration in body frame. (Y-axis)
		double 			AccelZ;				//m/s^2			Estimated acceleration in body frame. (Z-axis)
		double 			AngularRateX;		//rad/s			Estimated angular rate in body frame. (X-axis)
		double 			AngularRateY;		//rad/s			Estimated angular rate in body frame. (Y-axis)
		double 			AngularRateZ;		//rad/s			Estimated angular rate in body frame. (Z-axis)
		//INS
		double			time;				//sec			GPS time of week in seconds.
		double			AttUncertainty;		//deg			Uncertainty in attitude estimate.
		double			PosUncertainty;		//m				Uncertainty in position estimate.
		double			VelUncertainty;		//m/s			Uncertainty in velocity estimate.
		//Attitude						
		double 			ATT_YAW;			//deg			Yaw angle 
		double 			ATT_PITCH;			//deg			Pitch angle 
		double 			ATT_ROLL;			//deg			Pool angle 
	};
	//-----------------------------------------
	//		Struct:		Target
	//-----------------------------------------
	public value struct T_Target
	{
		double			target_reliability;		// [0 % :100 % ]
		double			target_range;			// [meters from radar]
		double			target_azimuth;			// [rad, 0 is LOS, positive counterclockwise]
		double			target_elevation;		// [rad, 0 is LOS, positive counterclockwise]
		double			target_doppler;			// [m / sec, positive is closing target]
		double			target_polarization;	// [rad, 0 is a horizontal, positive counterclockwise]
		T_SENSOR_data	SENSOR_data;			// Sensor data structure
		//double time;					// [sec, 0 is a week start]
		//double radar_lat;				// [degree]
		//double radar_lon;				// [degree]
		//double radar_alt;				// [meters]
		//double radar_att_roll;			// [rad, clockwise, relative to horizon, attitude]
		//double radar_att_pitch;			// [rad, clockwise, relative to horizon, attitude]
		//double radar_att_yaw;			// [rad, clockwise, relative to true north, attitude]
		//double radar_ins_roll;			// [rad, clockwise, relative to horizon, instantaneous]
		//double radar_ins_pitch;			// [rad, clockwise, relative to true north, instantaneous]
		//double radar_ins_yaw;			// [rad, 0 is a start position, instantaneous]
		//double radar_velocity_x;		// [m / sec, related to NED]
		//double radar_velocity_y;		// [m / sec, related to NED]
		//double radar_velocity_z;		// [m / sec, related to NED]
	};

	//-----------------------------------------
	//		Class:		MyForm
	//-----------------------------------------
	/// <summary>
	/// Summary for MyForm
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void);

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
			if (INITIAL_data != NULL)
			{
				free(INITIAL_data);
				INITIAL_data = NULL;
			}
		}

	private: System::Windows::Forms::Button^  B_CLEAR_WIRE;
	private: System::Windows::Forms::Button^  B_FLY;
	private: System::Windows::Forms::NumericUpDown^  B_HELICOPTER_SPEED;



	private: System::Windows::Forms::Label^  label1;





	private: System::Windows::Forms::Label^  label6;















	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::GroupBox^  groupBox3;



	private: System::Windows::Forms::GroupBox^  groupBox4;
	private: System::Windows::Forms::Panel^  panel1;
	private: System::Windows::Forms::RadioButton^  B_ROUTE_NEW;
	private: System::Windows::Forms::RadioButton^  B_ROUTE_REPEAT;


	private: System::Windows::Forms::Label^  label17;
	private: System::Windows::Forms::Panel^  panel2;
	private: System::Windows::Forms::Label^  B_SCALE;
	private: System::Windows::Forms::Panel^  B_PANEL;
	private: System::Windows::Forms::NumericUpDown^  B_WIRE_VARIANCE;
	private: System::Windows::Forms::NumericUpDown^  B_FALSE_ALARM_RATE;
	private: System::Windows::Forms::Label^  label13;
	private: System::Windows::Forms::Label^  label12;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label112;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label11;
	private: System::Windows::Forms::NumericUpDown^  B_RADAR_REFRESH;
	private: System::Windows::Forms::Label^  label10;
	private: System::Windows::Forms::NumericUpDown^  B_RADAR_FOV;
	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::Label^  label9;
	private: System::Windows::Forms::Label^  label8;
	private: System::Windows::Forms::NumericUpDown^  B_RADAR_RANGE;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::TextBox^  B_OBSTACLES_file;
	private: System::Windows::Forms::Button^  B_SAVE_OBSTACLES_FILE;

	private: System::Windows::Forms::Button^  B_BROWSE_OBSTACLES_FILE;
	private: System::Windows::Forms::OpenFileDialog^  B_OPEN_OBSTACLES_FILE;
	private: System::Windows::Forms::SaveFileDialog^  B_SAVE_OBSTACLES_FILE_DIALOGE;


	private: System::Windows::Forms::GroupBox^  groupBox5;
	private: System::Windows::Forms::Button^  B_STEP_FORWARD;
	private: System::Windows::Forms::CheckBox^  B_PLOT_DETECTIONS;
	private: System::Windows::Forms::Label^  label16;
	private: System::Windows::Forms::Label^  label15;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Label^  label14;
	private: System::Windows::Forms::TrackBar^  B_SCREEN_WIDTH;
	private: System::Windows::Forms::DataVisualization::Charting::Chart^  B_ERROR_FUNCTION;
	private: System::Windows::Forms::NumericUpDown^  B_RELIABILITY_THRESHOLD;
	private: System::Windows::Forms::Label^  label22;
	private: System::Windows::Forms::Label^  label23;
	private: System::Windows::Forms::NumericUpDown^  B_MAX_ERROR_RANGE;
	private: System::Windows::Forms::Label^  label20;
	private: System::Windows::Forms::Label^  label21;
	private: System::Windows::Forms::NumericUpDown^  B_WIRE_SEGMENT_LENGTH_METER;
	private: System::Windows::Forms::Label^  label24;
	private: System::Windows::Forms::Label^  label25;
	private: System::Windows::Forms::Label^  label27;
	private: System::Windows::Forms::Label^  label26;
	private: System::Windows::Forms::TextBox^  B_GPS_LONG;
	private: System::Windows::Forms::TextBox^  B_GPS_LAT;
	private: System::Windows::Forms::TabControl^  tabControl1;
	private: System::Windows::Forms::TabPage^  Viewer;

	private: System::Windows::Forms::TabPage^  tab_Scenario_generator;
	private: System::Windows::Forms::GroupBox^  groupBox8;
	private: System::Windows::Forms::Button^  B_LOAD_DETCTIONS;
	private: System::Windows::Forms::TextBox^  B_DETECTIONS_FOLDER;

	private: System::Windows::Forms::GroupBox^  groupBox7;
	private: System::Windows::Forms::Button^  B_LOAD_MAP_IMAGE;
	private: System::Windows::Forms::TextBox^  B_FILE_MAP;
	private: System::Windows::Forms::GroupBox^  groupBox6;
	private: System::Windows::Forms::Button^  B_LOAD_TOM;
	private: System::Windows::Forms::TextBox^  B_FILE_TOM;
	private: System::Windows::Forms::GroupBox^  groupBox9;
	private: System::Windows::Forms::GroupBox^  groupBox10;
	private: System::Windows::Forms::Button^  B_SAVE_DETECTIONS_DIRECTORY;
	private: System::Windows::Forms::TextBox^  B_EXPERIMENT_DIRECTORY;



	private: System::Windows::Forms::Label^  label50;
	private: System::Windows::Forms::TextBox^  B_MESSAGE;
	private: System::Windows::Forms::FolderBrowserDialog^  B_BROWSE_DIR;
	private: System::Windows::Forms::GroupBox^  groupBox11;
	private: System::Windows::Forms::Button^  B_VIEWER_STEP;
	private: System::Windows::Forms::Button^  B_VIEWER_PLAY;



	protected:

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;
#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			System::Windows::Forms::DataVisualization::Charting::ChartArea^  chartArea1 = (gcnew System::Windows::Forms::DataVisualization::Charting::ChartArea());
			System::Windows::Forms::DataVisualization::Charting::Series^  series1 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(MyForm::typeid));
			this->B_CLEAR_WIRE = (gcnew System::Windows::Forms::Button());
			this->B_FLY = (gcnew System::Windows::Forms::Button());
			this->B_HELICOPTER_SPEED = (gcnew System::Windows::Forms::NumericUpDown());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
			this->B_WIRE_SEGMENT_LENGTH_METER = (gcnew System::Windows::Forms::NumericUpDown());
			this->label24 = (gcnew System::Windows::Forms::Label());
			this->label25 = (gcnew System::Windows::Forms::Label());
			this->B_RELIABILITY_THRESHOLD = (gcnew System::Windows::Forms::NumericUpDown());
			this->label22 = (gcnew System::Windows::Forms::Label());
			this->label23 = (gcnew System::Windows::Forms::Label());
			this->B_MAX_ERROR_RANGE = (gcnew System::Windows::Forms::NumericUpDown());
			this->label20 = (gcnew System::Windows::Forms::Label());
			this->label21 = (gcnew System::Windows::Forms::Label());
			this->groupBox4 = (gcnew System::Windows::Forms::GroupBox());
			this->B_SAVE_OBSTACLES_FILE = (gcnew System::Windows::Forms::Button());
			this->B_BROWSE_OBSTACLES_FILE = (gcnew System::Windows::Forms::Button());
			this->B_OBSTACLES_file = (gcnew System::Windows::Forms::TextBox());
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->B_ROUTE_NEW = (gcnew System::Windows::Forms::RadioButton());
			this->B_ROUTE_REPEAT = (gcnew System::Windows::Forms::RadioButton());
			this->label17 = (gcnew System::Windows::Forms::Label());
			this->panel2 = (gcnew System::Windows::Forms::Panel());
			this->B_SCALE = (gcnew System::Windows::Forms::Label());
			this->B_PANEL = (gcnew System::Windows::Forms::Panel());
			this->B_WIRE_VARIANCE = (gcnew System::Windows::Forms::NumericUpDown());
			this->B_FALSE_ALARM_RATE = (gcnew System::Windows::Forms::NumericUpDown());
			this->label13 = (gcnew System::Windows::Forms::Label());
			this->label12 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label112 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label11 = (gcnew System::Windows::Forms::Label());
			this->B_RADAR_REFRESH = (gcnew System::Windows::Forms::NumericUpDown());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->B_RADAR_FOV = (gcnew System::Windows::Forms::NumericUpDown());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->B_RADAR_RANGE = (gcnew System::Windows::Forms::NumericUpDown());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->B_PLOT_DETECTIONS = (gcnew System::Windows::Forms::CheckBox());
			this->label27 = (gcnew System::Windows::Forms::Label());
			this->label26 = (gcnew System::Windows::Forms::Label());
			this->B_GPS_LONG = (gcnew System::Windows::Forms::TextBox());
			this->B_GPS_LAT = (gcnew System::Windows::Forms::TextBox());
			this->B_OPEN_OBSTACLES_FILE = (gcnew System::Windows::Forms::OpenFileDialog());
			this->B_SAVE_OBSTACLES_FILE_DIALOGE = (gcnew System::Windows::Forms::SaveFileDialog());
			this->groupBox5 = (gcnew System::Windows::Forms::GroupBox());
			this->label50 = (gcnew System::Windows::Forms::Label());
			this->B_MESSAGE = (gcnew System::Windows::Forms::TextBox());
			this->label16 = (gcnew System::Windows::Forms::Label());
			this->label15 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label14 = (gcnew System::Windows::Forms::Label());
			this->B_SCREEN_WIDTH = (gcnew System::Windows::Forms::TrackBar());
			this->B_STEP_FORWARD = (gcnew System::Windows::Forms::Button());
			this->B_ERROR_FUNCTION = (gcnew System::Windows::Forms::DataVisualization::Charting::Chart());
			this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
			this->Viewer = (gcnew System::Windows::Forms::TabPage());
			this->groupBox8 = (gcnew System::Windows::Forms::GroupBox());
			this->B_LOAD_DETCTIONS = (gcnew System::Windows::Forms::Button());
			this->B_DETECTIONS_FOLDER = (gcnew System::Windows::Forms::TextBox());
			this->groupBox7 = (gcnew System::Windows::Forms::GroupBox());
			this->B_LOAD_MAP_IMAGE = (gcnew System::Windows::Forms::Button());
			this->B_FILE_MAP = (gcnew System::Windows::Forms::TextBox());
			this->groupBox6 = (gcnew System::Windows::Forms::GroupBox());
			this->B_LOAD_TOM = (gcnew System::Windows::Forms::Button());
			this->B_FILE_TOM = (gcnew System::Windows::Forms::TextBox());
			this->tab_Scenario_generator = (gcnew System::Windows::Forms::TabPage());
			this->groupBox10 = (gcnew System::Windows::Forms::GroupBox());
			this->B_SAVE_DETECTIONS_DIRECTORY = (gcnew System::Windows::Forms::Button());
			this->B_EXPERIMENT_DIRECTORY = (gcnew System::Windows::Forms::TextBox());
			this->groupBox9 = (gcnew System::Windows::Forms::GroupBox());
			this->B_BROWSE_DIR = (gcnew System::Windows::Forms::FolderBrowserDialog());
			this->groupBox11 = (gcnew System::Windows::Forms::GroupBox());
			this->B_VIEWER_STEP = (gcnew System::Windows::Forms::Button());
			this->B_VIEWER_PLAY = (gcnew System::Windows::Forms::Button());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->B_HELICOPTER_SPEED))->BeginInit();
			this->groupBox2->SuspendLayout();
			this->groupBox3->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->B_WIRE_SEGMENT_LENGTH_METER))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->B_RELIABILITY_THRESHOLD))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->B_MAX_ERROR_RANGE))->BeginInit();
			this->groupBox4->SuspendLayout();
			this->panel1->SuspendLayout();
			this->B_PANEL->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->B_WIRE_VARIANCE))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->B_FALSE_ALARM_RATE))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->B_RADAR_REFRESH))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->B_RADAR_FOV))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->B_RADAR_RANGE))->BeginInit();
			this->groupBox1->SuspendLayout();
			this->groupBox5->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->B_SCREEN_WIDTH))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->B_ERROR_FUNCTION))->BeginInit();
			this->tabControl1->SuspendLayout();
			this->Viewer->SuspendLayout();
			this->groupBox8->SuspendLayout();
			this->groupBox7->SuspendLayout();
			this->groupBox6->SuspendLayout();
			this->tab_Scenario_generator->SuspendLayout();
			this->groupBox10->SuspendLayout();
			this->groupBox9->SuspendLayout();
			this->groupBox11->SuspendLayout();
			this->SuspendLayout();
			// 
			// B_CLEAR_WIRE
			// 
			this->B_CLEAR_WIRE->ForeColor = System::Drawing::Color::Black;
			this->B_CLEAR_WIRE->Location = System::Drawing::Point(15, 44);
			this->B_CLEAR_WIRE->Name = L"B_CLEAR_WIRE";
			this->B_CLEAR_WIRE->Size = System::Drawing::Size(93, 27);
			this->B_CLEAR_WIRE->TabIndex = 1;
			this->B_CLEAR_WIRE->Text = L"Clear";
			this->B_CLEAR_WIRE->UseVisualStyleBackColor = true;
			this->B_CLEAR_WIRE->Click += gcnew System::EventHandler(this, &MyForm::B_CLEAR_WIRE_Click);
			// 
			// B_FLY
			// 
			this->B_FLY->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->B_FLY->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->B_FLY->ForeColor = System::Drawing::Color::Black;
			this->B_FLY->Location = System::Drawing::Point(16, 17);
			this->B_FLY->Name = L"B_FLY";
			this->B_FLY->Size = System::Drawing::Size(69, 48);
			this->B_FLY->TabIndex = 2;
			this->B_FLY->Text = L"Fly";
			this->B_FLY->UseVisualStyleBackColor = true;
			this->B_FLY->Click += gcnew System::EventHandler(this, &MyForm::B_FLY_Click);
			// 
			// B_HELICOPTER_SPEED
			// 
			this->B_HELICOPTER_SPEED->Location = System::Drawing::Point(96, 121);
			this->B_HELICOPTER_SPEED->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 200, 0, 0, 0 });
			this->B_HELICOPTER_SPEED->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
			this->B_HELICOPTER_SPEED->Name = L"B_HELICOPTER_SPEED";
			this->B_HELICOPTER_SPEED->Size = System::Drawing::Size(76, 20);
			this->B_HELICOPTER_SPEED->TabIndex = 3;
			this->B_HELICOPTER_SPEED->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
			this->B_HELICOPTER_SPEED->ValueChanged += gcnew System::EventHandler(this, &MyForm::B_SPEED_ValueChanged);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(46, 123);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(44, 13);
			this->label1->TabIndex = 6;
			this->label1->Text = L"Velocity";
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(178, 123);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(39, 13);
			this->label6->TabIndex = 11;
			this->label6->Text = L"m/Sec";
			// 
			// groupBox2
			// 
			this->groupBox2->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(128)), static_cast<System::Int32>(static_cast<System::Byte>(128)),
				static_cast<System::Int32>(static_cast<System::Byte>(255)));
			this->groupBox2->Controls->Add(this->B_PLOT_DETECTIONS);
			this->groupBox2->Controls->Add(this->panel1);
			this->groupBox2->Controls->Add(this->B_STEP_FORWARD);
			this->groupBox2->Controls->Add(this->B_FLY);
			this->groupBox2->ForeColor = System::Drawing::Color::White;
			this->groupBox2->Location = System::Drawing::Point(1, 96);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(330, 73);
			this->groupBox2->TabIndex = 24;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Experiment";
			// 
			// groupBox3
			// 
			this->groupBox3->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(128)), static_cast<System::Int32>(static_cast<System::Byte>(128)),
				static_cast<System::Int32>(static_cast<System::Byte>(255)));
			this->groupBox3->Controls->Add(this->B_WIRE_SEGMENT_LENGTH_METER);
			this->groupBox3->Controls->Add(this->label24);
			this->groupBox3->Controls->Add(this->label25);
			this->groupBox3->Controls->Add(this->B_RELIABILITY_THRESHOLD);
			this->groupBox3->Controls->Add(this->label22);
			this->groupBox3->Controls->Add(this->label23);
			this->groupBox3->Controls->Add(this->B_MAX_ERROR_RANGE);
			this->groupBox3->Controls->Add(this->label20);
			this->groupBox3->Controls->Add(this->label21);
			this->groupBox3->ForeColor = System::Drawing::Color::White;
			this->groupBox3->Location = System::Drawing::Point(1, 1);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(330, 89);
			this->groupBox3->TabIndex = 25;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = L"Detection algorithm";
			// 
			// B_WIRE_SEGMENT_LENGTH_METER
			// 
			this->B_WIRE_SEGMENT_LENGTH_METER->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) { 10, 0, 0, 0 });
			this->B_WIRE_SEGMENT_LENGTH_METER->Location = System::Drawing::Point(115, 63);
			this->B_WIRE_SEGMENT_LENGTH_METER->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1000, 0, 0, 0 });
			this->B_WIRE_SEGMENT_LENGTH_METER->Name = L"B_WIRE_SEGMENT_LENGTH_METER";
			this->B_WIRE_SEGMENT_LENGTH_METER->Size = System::Drawing::Size(76, 20);
			this->B_WIRE_SEGMENT_LENGTH_METER->TabIndex = 29;
			this->B_WIRE_SEGMENT_LENGTH_METER->ValueChanged += gcnew System::EventHandler(this, &MyForm::B_WIRE_SEGMENT_LENGTH_METER_ValueChanged);
			// 
			// label24
			// 
			this->label24->AutoSize = true;
			this->label24->Location = System::Drawing::Point(192, 66);
			this->label24->Name = L"label24";
			this->label24->Size = System::Drawing::Size(15, 13);
			this->label24->TabIndex = 31;
			this->label24->Text = L"m";
			// 
			// label25
			// 
			this->label25->AutoSize = true;
			this->label25->Location = System::Drawing::Point(11, 65);
			this->label25->Name = L"label25";
			this->label25->Size = System::Drawing::Size(106, 13);
			this->label25->TabIndex = 30;
			this->label25->Text = L"Typical wire segment";
			// 
			// B_RELIABILITY_THRESHOLD
			// 
			this->B_RELIABILITY_THRESHOLD->Location = System::Drawing::Point(115, 42);
			this->B_RELIABILITY_THRESHOLD->Name = L"B_RELIABILITY_THRESHOLD";
			this->B_RELIABILITY_THRESHOLD->Size = System::Drawing::Size(76, 20);
			this->B_RELIABILITY_THRESHOLD->TabIndex = 26;
			this->B_RELIABILITY_THRESHOLD->ValueChanged += gcnew System::EventHandler(this, &MyForm::B_RELIABILITY_THRESHOLD_ValueChanged);
			// 
			// label22
			// 
			this->label22->AutoSize = true;
			this->label22->Location = System::Drawing::Point(192, 45);
			this->label22->Name = L"label22";
			this->label22->Size = System::Drawing::Size(15, 13);
			this->label22->TabIndex = 28;
			this->label22->Text = L"%";
			// 
			// label23
			// 
			this->label23->AutoSize = true;
			this->label23->Location = System::Drawing::Point(19, 44);
			this->label23->Name = L"label23";
			this->label23->Size = System::Drawing::Size(97, 13);
			this->label23->TabIndex = 27;
			this->label23->Text = L"Reliability threshold";
			// 
			// B_MAX_ERROR_RANGE
			// 
			this->B_MAX_ERROR_RANGE->Location = System::Drawing::Point(115, 21);
			this->B_MAX_ERROR_RANGE->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1000, 0, 0, 0 });
			this->B_MAX_ERROR_RANGE->Name = L"B_MAX_ERROR_RANGE";
			this->B_MAX_ERROR_RANGE->Size = System::Drawing::Size(76, 20);
			this->B_MAX_ERROR_RANGE->TabIndex = 23;
			this->B_MAX_ERROR_RANGE->ValueChanged += gcnew System::EventHandler(this, &MyForm::B_MAX_ERROR_RANGE_ValueChanged);
			// 
			// label20
			// 
			this->label20->AutoSize = true;
			this->label20->Location = System::Drawing::Point(192, 23);
			this->label20->Name = L"label20";
			this->label20->Size = System::Drawing::Size(15, 13);
			this->label20->TabIndex = 25;
			this->label20->Text = L"m";
			// 
			// label21
			// 
			this->label21->AutoSize = true;
			this->label21->Location = System::Drawing::Point(35, 23);
			this->label21->Name = L"label21";
			this->label21->Size = System::Drawing::Size(81, 13);
			this->label21->TabIndex = 24;
			this->label21->Text = L"Max error range";
			// 
			// groupBox4
			// 
			this->groupBox4->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(128)), static_cast<System::Int32>(static_cast<System::Byte>(128)),
				static_cast<System::Int32>(static_cast<System::Byte>(255)));
			this->groupBox4->Controls->Add(this->B_SAVE_OBSTACLES_FILE);
			this->groupBox4->Controls->Add(this->B_BROWSE_OBSTACLES_FILE);
			this->groupBox4->Controls->Add(this->B_OBSTACLES_file);
			this->groupBox4->Controls->Add(this->B_CLEAR_WIRE);
			this->groupBox4->ForeColor = System::Drawing::Color::White;
			this->groupBox4->Location = System::Drawing::Point(1, 403);
			this->groupBox4->Name = L"groupBox4";
			this->groupBox4->Size = System::Drawing::Size(330, 77);
			this->groupBox4->TabIndex = 26;
			this->groupBox4->TabStop = false;
			this->groupBox4->Text = L"True Obstacles-map (TOM):";
			// 
			// B_SAVE_OBSTACLES_FILE
			// 
			this->B_SAVE_OBSTACLES_FILE->ForeColor = System::Drawing::Color::Black;
			this->B_SAVE_OBSTACLES_FILE->Location = System::Drawing::Point(220, 44);
			this->B_SAVE_OBSTACLES_FILE->Name = L"B_SAVE_OBSTACLES_FILE";
			this->B_SAVE_OBSTACLES_FILE->Size = System::Drawing::Size(93, 27);
			this->B_SAVE_OBSTACLES_FILE->TabIndex = 9;
			this->B_SAVE_OBSTACLES_FILE->Text = L"Save as";
			this->B_SAVE_OBSTACLES_FILE->UseVisualStyleBackColor = true;
			this->B_SAVE_OBSTACLES_FILE->Click += gcnew System::EventHandler(this, &MyForm::B_SAVE_OBSTACLES_FILE_Click);
			// 
			// B_BROWSE_OBSTACLES_FILE
			// 
			this->B_BROWSE_OBSTACLES_FILE->ForeColor = System::Drawing::Color::Black;
			this->B_BROWSE_OBSTACLES_FILE->Location = System::Drawing::Point(115, 44);
			this->B_BROWSE_OBSTACLES_FILE->Name = L"B_BROWSE_OBSTACLES_FILE";
			this->B_BROWSE_OBSTACLES_FILE->Size = System::Drawing::Size(93, 27);
			this->B_BROWSE_OBSTACLES_FILE->TabIndex = 7;
			this->B_BROWSE_OBSTACLES_FILE->Text = L"Load";
			this->B_BROWSE_OBSTACLES_FILE->UseVisualStyleBackColor = true;
			this->B_BROWSE_OBSTACLES_FILE->Click += gcnew System::EventHandler(this, &MyForm::B_BROWSE_OBSTACLES_FILE_Click);
			// 
			// B_OBSTACLES_file
			// 
			this->B_OBSTACLES_file->Location = System::Drawing::Point(16, 18);
			this->B_OBSTACLES_file->Name = L"B_OBSTACLES_file";
			this->B_OBSTACLES_file->Size = System::Drawing::Size(296, 20);
			this->B_OBSTACLES_file->TabIndex = 6;
			// 
			// panel1
			// 
			this->panel1->Controls->Add(this->B_ROUTE_NEW);
			this->panel1->Controls->Add(this->B_ROUTE_REPEAT);
			this->panel1->Location = System::Drawing::Point(216, 28);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(104, 39);
			this->panel1->TabIndex = 5;
			// 
			// B_ROUTE_NEW
			// 
			this->B_ROUTE_NEW->AutoSize = true;
			this->B_ROUTE_NEW->Checked = true;
			this->B_ROUTE_NEW->Location = System::Drawing::Point(5, 2);
			this->B_ROUTE_NEW->Name = L"B_ROUTE_NEW";
			this->B_ROUTE_NEW->Size = System::Drawing::Size(74, 17);
			this->B_ROUTE_NEW->TabIndex = 3;
			this->B_ROUTE_NEW->TabStop = true;
			this->B_ROUTE_NEW->Text = L"New route";
			this->B_ROUTE_NEW->UseVisualStyleBackColor = true;
			this->B_ROUTE_NEW->CheckedChanged += gcnew System::EventHandler(this, &MyForm::B_ROUTE_NEW_CheckedChanged);
			// 
			// B_ROUTE_REPEAT
			// 
			this->B_ROUTE_REPEAT->AutoSize = true;
			this->B_ROUTE_REPEAT->Location = System::Drawing::Point(5, 20);
			this->B_ROUTE_REPEAT->Name = L"B_ROUTE_REPEAT";
			this->B_ROUTE_REPEAT->Size = System::Drawing::Size(87, 17);
			this->B_ROUTE_REPEAT->TabIndex = 4;
			this->B_ROUTE_REPEAT->Text = L"Repeat route";
			this->B_ROUTE_REPEAT->UseVisualStyleBackColor = true;
			// 
			// label17
			// 
			this->label17->AutoSize = true;
			this->label17->BackColor = System::Drawing::SystemColors::ActiveCaptionText;
			this->label17->ForeColor = System::Drawing::Color::White;
			this->label17->Location = System::Drawing::Point(516, 571);
			this->label17->Name = L"label17";
			this->label17->Size = System::Drawing::Size(21, 13);
			this->label17->TabIndex = 27;
			this->label17->Text = L"km";
			// 
			// panel2
			// 
			this->panel2->BackColor = System::Drawing::Color::White;
			this->panel2->ForeColor = System::Drawing::Color::White;
			this->panel2->Location = System::Drawing::Point(487, 564);
			this->panel2->Name = L"panel2";
			this->panel2->Size = System::Drawing::Size(60, 4);
			this->panel2->TabIndex = 28;
			// 
			// B_SCALE
			// 
			this->B_SCALE->AutoSize = true;
			this->B_SCALE->BackColor = System::Drawing::SystemColors::ActiveCaptionText;
			this->B_SCALE->ForeColor = System::Drawing::Color::White;
			this->B_SCALE->Location = System::Drawing::Point(494, 571);
			this->B_SCALE->Name = L"B_SCALE";
			this->B_SCALE->Size = System::Drawing::Size(19, 13);
			this->B_SCALE->TabIndex = 9;
			this->B_SCALE->Text = L"15";
			// 
			// B_PANEL
			// 
			this->B_PANEL->BackColor = System::Drawing::SystemColors::ActiveCaptionText;
			this->B_PANEL->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->B_PANEL->Controls->Add(this->panel2);
			this->B_PANEL->Controls->Add(this->B_SCALE);
			this->B_PANEL->Controls->Add(this->label17);
			this->B_PANEL->Location = System::Drawing::Point(2, 3);
			this->B_PANEL->Name = L"B_PANEL";
			this->B_PANEL->Size = System::Drawing::Size(600, 600);
			this->B_PANEL->TabIndex = 0;
			this->B_PANEL->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &MyForm::B_PANEL_Paint);
			this->B_PANEL->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::B_PANEL_MouseClick);
			this->B_PANEL->MouseLeave += gcnew System::EventHandler(this, &MyForm::B_PANEL_MouseLeave);
			this->B_PANEL->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::B_PANEL_MouseMove);
			// 
			// B_WIRE_VARIANCE
			// 
			this->B_WIRE_VARIANCE->Location = System::Drawing::Point(96, 79);
			this->B_WIRE_VARIANCE->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 300, 0, 0, 0 });
			this->B_WIRE_VARIANCE->Name = L"B_WIRE_VARIANCE";
			this->B_WIRE_VARIANCE->Size = System::Drawing::Size(76, 20);
			this->B_WIRE_VARIANCE->TabIndex = 4;
			this->B_WIRE_VARIANCE->ValueChanged += gcnew System::EventHandler(this, &MyForm::B_WIRE_VARIANCE_ValueChanged);
			// 
			// B_FALSE_ALARM_RATE
			// 
			this->B_FALSE_ALARM_RATE->Location = System::Drawing::Point(96, 100);
			this->B_FALSE_ALARM_RATE->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 10000, 0, 0, 0 });
			this->B_FALSE_ALARM_RATE->Name = L"B_FALSE_ALARM_RATE";
			this->B_FALSE_ALARM_RATE->Size = System::Drawing::Size(76, 20);
			this->B_FALSE_ALARM_RATE->TabIndex = 5;
			this->B_FALSE_ALARM_RATE->ValueChanged += gcnew System::EventHandler(this, &MyForm::B_FALSE_ALARM_RATE_ValueChanged);
			// 
			// label13
			// 
			this->label13->AutoSize = true;
			this->label13->Location = System::Drawing::Point(62, 39);
			this->label13->Name = L"label13";
			this->label13->Size = System::Drawing::Size(28, 13);
			this->label13->TabIndex = 18;
			this->label13->Text = L"FOV";
			// 
			// label12
			// 
			this->label12->AutoSize = true;
			this->label12->Location = System::Drawing::Point(24, 60);
			this->label12->Name = L"label12";
			this->label12->Size = System::Drawing::Size(66, 13);
			this->label12->TabIndex = 19;
			this->label12->Text = L"Refresh time";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(2, 81);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(88, 13);
			this->label2->TabIndex = 7;
			this->label2->Text = L"Position variance";
			// 
			// label112
			// 
			this->label112->AutoSize = true;
			this->label112->Location = System::Drawing::Point(51, 18);
			this->label112->Name = L"label112";
			this->label112->Size = System::Drawing::Size(39, 13);
			this->label112->TabIndex = 17;
			this->label112->Text = L"Range";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(9, 102);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(81, 13);
			this->label3->TabIndex = 8;
			this->label3->Text = L"False alarm rate";
			// 
			// label11
			// 
			this->label11->AutoSize = true;
			this->label11->Location = System::Drawing::Point(178, 18);
			this->label11->Name = L"label11";
			this->label11->Size = System::Drawing::Size(15, 13);
			this->label11->TabIndex = 20;
			this->label11->Text = L"m";
			// 
			// B_RADAR_REFRESH
			// 
			this->B_RADAR_REFRESH->Location = System::Drawing::Point(96, 58);
			this->B_RADAR_REFRESH->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
			this->B_RADAR_REFRESH->Name = L"B_RADAR_REFRESH";
			this->B_RADAR_REFRESH->Size = System::Drawing::Size(76, 20);
			this->B_RADAR_REFRESH->TabIndex = 16;
			this->B_RADAR_REFRESH->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
			this->B_RADAR_REFRESH->ValueChanged += gcnew System::EventHandler(this, &MyForm::B_RADAR_REFRESH_ValueChanged);
			// 
			// label10
			// 
			this->label10->AutoSize = true;
			this->label10->Location = System::Drawing::Point(178, 39);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(30, 13);
			this->label10->TabIndex = 21;
			this->label10->Text = L"degs";
			// 
			// B_RADAR_FOV
			// 
			this->B_RADAR_FOV->Location = System::Drawing::Point(96, 37);
			this->B_RADAR_FOV->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 360, 0, 0, 0 });
			this->B_RADAR_FOV->Name = L"B_RADAR_FOV";
			this->B_RADAR_FOV->Size = System::Drawing::Size(76, 20);
			this->B_RADAR_FOV->TabIndex = 15;
			this->B_RADAR_FOV->ValueChanged += gcnew System::EventHandler(this, &MyForm::B_RADAR_FOV_ValueChanged);
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(178, 81);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(15, 13);
			this->label7->TabIndex = 12;
			this->label7->Text = L"m";
			// 
			// label9
			// 
			this->label9->AutoSize = true;
			this->label9->Location = System::Drawing::Point(178, 61);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(60, 13);
			this->label9->TabIndex = 22;
			this->label9->Text = L"per second";
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Location = System::Drawing::Point(178, 102);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(46, 13);
			this->label8->TabIndex = 13;
			this->label8->Text = L"per hour";
			// 
			// B_RADAR_RANGE
			// 
			this->B_RADAR_RANGE->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) { 100, 0, 0, 0 });
			this->B_RADAR_RANGE->Location = System::Drawing::Point(96, 16);
			this->B_RADAR_RANGE->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 3000, 0, 0, 0 });
			this->B_RADAR_RANGE->Name = L"B_RADAR_RANGE";
			this->B_RADAR_RANGE->Size = System::Drawing::Size(76, 20);
			this->B_RADAR_RANGE->TabIndex = 14;
			this->B_RADAR_RANGE->ValueChanged += gcnew System::EventHandler(this, &MyForm::B_RADAR_RANGE_ValueChanged);
			// 
			// groupBox1
			// 
			this->groupBox1->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(128)), static_cast<System::Int32>(static_cast<System::Byte>(128)),
				static_cast<System::Int32>(static_cast<System::Byte>(255)));
			this->groupBox1->Controls->Add(this->B_HELICOPTER_SPEED);
			this->groupBox1->Controls->Add(this->label1);
			this->groupBox1->Controls->Add(this->B_RADAR_RANGE);
			this->groupBox1->Controls->Add(this->label6);
			this->groupBox1->Controls->Add(this->label8);
			this->groupBox1->Controls->Add(this->label9);
			this->groupBox1->Controls->Add(this->label7);
			this->groupBox1->Controls->Add(this->B_RADAR_FOV);
			this->groupBox1->Controls->Add(this->label10);
			this->groupBox1->Controls->Add(this->B_RADAR_REFRESH);
			this->groupBox1->Controls->Add(this->label11);
			this->groupBox1->Controls->Add(this->label3);
			this->groupBox1->Controls->Add(this->label112);
			this->groupBox1->Controls->Add(this->label2);
			this->groupBox1->Controls->Add(this->label12);
			this->groupBox1->Controls->Add(this->label13);
			this->groupBox1->Controls->Add(this->B_FALSE_ALARM_RATE);
			this->groupBox1->Controls->Add(this->B_WIRE_VARIANCE);
			this->groupBox1->ForeColor = System::Drawing::Color::White;
			this->groupBox1->Location = System::Drawing::Point(2, 175);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(330, 147);
			this->groupBox1->TabIndex = 23;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Radar parameters";
			// 
			// B_PLOT_DETECTIONS
			// 
			this->B_PLOT_DETECTIONS->AutoSize = true;
			this->B_PLOT_DETECTIONS->Location = System::Drawing::Point(220, 10);
			this->B_PLOT_DETECTIONS->Name = L"B_PLOT_DETECTIONS";
			this->B_PLOT_DETECTIONS->Size = System::Drawing::Size(105, 17);
			this->B_PLOT_DETECTIONS->TabIndex = 7;
			this->B_PLOT_DETECTIONS->Text = L"Show detections";
			this->B_PLOT_DETECTIONS->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			this->B_PLOT_DETECTIONS->UseVisualStyleBackColor = true;
			this->B_PLOT_DETECTIONS->CheckedChanged += gcnew System::EventHandler(this, &MyForm::B_PLOT_DETECTIONS_CheckedChanged);
			// 
			// label27
			// 
			this->label27->AutoSize = true;
			this->label27->Location = System::Drawing::Point(141, 38);
			this->label27->Name = L"label27";
			this->label27->Size = System::Drawing::Size(65, 13);
			this->label27->TabIndex = 25;
			this->label27->Text = L"GPS LONG.";
			// 
			// label26
			// 
			this->label26->AutoSize = true;
			this->label26->Location = System::Drawing::Point(151, 16);
			this->label26->Name = L"label26";
			this->label26->Size = System::Drawing::Size(55, 13);
			this->label26->TabIndex = 24;
			this->label26->Text = L"GPS LAT.";
			// 
			// B_GPS_LONG
			// 
			this->B_GPS_LONG->Location = System::Drawing::Point(211, 32);
			this->B_GPS_LONG->Name = L"B_GPS_LONG";
			this->B_GPS_LONG->Size = System::Drawing::Size(76, 20);
			this->B_GPS_LONG->TabIndex = 23;
			this->B_GPS_LONG->TextChanged += gcnew System::EventHandler(this, &MyForm::B_GPS_LONG_TextChanged);
			// 
			// B_GPS_LAT
			// 
			this->B_GPS_LAT->Location = System::Drawing::Point(211, 11);
			this->B_GPS_LAT->Name = L"B_GPS_LAT";
			this->B_GPS_LAT->Size = System::Drawing::Size(76, 20);
			this->B_GPS_LAT->TabIndex = 22;
			this->B_GPS_LAT->TextChanged += gcnew System::EventHandler(this, &MyForm::B_GPS_LAT_TextChanged);
			// 
			// B_OPEN_OBSTACLES_FILE
			// 
			this->B_OPEN_OBSTACLES_FILE->FileName = L"openFileDialog1";
			// 
			// groupBox5
			// 
			this->groupBox5->BackColor = System::Drawing::Color::Blue;
			this->groupBox5->Controls->Add(this->label27);
			this->groupBox5->Controls->Add(this->label50);
			this->groupBox5->Controls->Add(this->B_MESSAGE);
			this->groupBox5->Controls->Add(this->label26);
			this->groupBox5->Controls->Add(this->label16);
			this->groupBox5->Controls->Add(this->label15);
			this->groupBox5->Controls->Add(this->B_GPS_LONG);
			this->groupBox5->Controls->Add(this->B_GPS_LAT);
			this->groupBox5->Controls->Add(this->label5);
			this->groupBox5->Controls->Add(this->label14);
			this->groupBox5->Controls->Add(this->B_SCREEN_WIDTH);
			this->groupBox5->ForeColor = System::Drawing::Color::White;
			this->groupBox5->Location = System::Drawing::Point(3, 605);
			this->groupBox5->Name = L"groupBox5";
			this->groupBox5->Size = System::Drawing::Size(600, 81);
			this->groupBox5->TabIndex = 27;
			this->groupBox5->TabStop = false;
			this->groupBox5->Text = L"Screen";
			// 
			// label50
			// 
			this->label50->AutoSize = true;
			this->label50->ForeColor = System::Drawing::Color::White;
			this->label50->Location = System::Drawing::Point(158, 60);
			this->label50->Name = L"label50";
			this->label50->Size = System::Drawing::Size(53, 13);
			this->label50->TabIndex = 23;
			this->label50->Text = L"Message:";
			// 
			// B_MESSAGE
			// 
			this->B_MESSAGE->BackColor = System::Drawing::Color::Black;
			this->B_MESSAGE->ForeColor = System::Drawing::Color::Yellow;
			this->B_MESSAGE->Location = System::Drawing::Point(211, 58);
			this->B_MESSAGE->Name = L"B_MESSAGE";
			this->B_MESSAGE->ReadOnly = true;
			this->B_MESSAGE->Size = System::Drawing::Size(384, 20);
			this->B_MESSAGE->TabIndex = 22;
			// 
			// label16
			// 
			this->label16->AutoSize = true;
			this->label16->Location = System::Drawing::Point(540, 42);
			this->label16->Name = L"label16";
			this->label16->Size = System::Drawing::Size(36, 13);
			this->label16->TabIndex = 21;
			this->label16->Text = L"15 km";
			// 
			// label15
			// 
			this->label15->AutoSize = true;
			this->label15->Location = System::Drawing::Point(413, 42);
			this->label15->Name = L"label15";
			this->label15->Size = System::Drawing::Size(30, 13);
			this->label15->TabIndex = 20;
			this->label15->Text = L"2 km";
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(561, 23);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(21, 13);
			this->label5->TabIndex = 19;
			this->label5->Text = L"km";
			// 
			// label14
			// 
			this->label14->AutoSize = true;
			this->label14->Location = System::Drawing::Point(341, 23);
			this->label14->Name = L"label14";
			this->label14->Size = System::Drawing::Size(69, 13);
			this->label14->TabIndex = 18;
			this->label14->Text = L"Screen width";
			// 
			// B_SCREEN_WIDTH
			// 
			this->B_SCREEN_WIDTH->Location = System::Drawing::Point(416, 13);
			this->B_SCREEN_WIDTH->Maximum = 15;
			this->B_SCREEN_WIDTH->Minimum = 2;
			this->B_SCREEN_WIDTH->Name = L"B_SCREEN_WIDTH";
			this->B_SCREEN_WIDTH->Size = System::Drawing::Size(145, 45);
			this->B_SCREEN_WIDTH->TabIndex = 17;
			this->B_SCREEN_WIDTH->Value = 10;
			this->B_SCREEN_WIDTH->Scroll += gcnew System::EventHandler(this, &MyForm::B_SCREEN_WIDTH_Scroll);
			// 
			// B_STEP_FORWARD
			// 
			this->B_STEP_FORWARD->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->B_STEP_FORWARD->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->B_STEP_FORWARD->ForeColor = System::Drawing::Color::Black;
			this->B_STEP_FORWARD->Location = System::Drawing::Point(101, 17);
			this->B_STEP_FORWARD->Name = L"B_STEP_FORWARD";
			this->B_STEP_FORWARD->Size = System::Drawing::Size(73, 48);
			this->B_STEP_FORWARD->TabIndex = 6;
			this->B_STEP_FORWARD->Text = L"Step";
			this->B_STEP_FORWARD->UseVisualStyleBackColor = true;
			this->B_STEP_FORWARD->Click += gcnew System::EventHandler(this, &MyForm::B_STEP_FORWARD_Click);
			// 
			// B_ERROR_FUNCTION
			// 
			this->B_ERROR_FUNCTION->BackColor = System::Drawing::Color::Blue;
			chartArea1->AxisX->LabelStyle->ForeColor = System::Drawing::Color::White;
			chartArea1->AxisX->LineColor = System::Drawing::Color::White;
			chartArea1->AxisX->MajorGrid->LineColor = System::Drawing::Color::White;
			chartArea1->AxisX->MajorTickMark->LineColor = System::Drawing::Color::White;
			chartArea1->AxisX->Maximum = 600;
			chartArea1->AxisX->Minimum = 0;
			chartArea1->AxisX->Title = L"Step #";
			chartArea1->AxisX->TitleForeColor = System::Drawing::Color::White;
			chartArea1->AxisY->LabelStyle->ForeColor = System::Drawing::Color::White;
			chartArea1->AxisY->LineColor = System::Drawing::Color::White;
			chartArea1->AxisY->MajorGrid->LineColor = System::Drawing::Color::White;
			chartArea1->AxisY->MajorTickMark->LineColor = System::Drawing::Color::White;
			chartArea1->AxisY->Maximum = 1;
			chartArea1->AxisY->Minimum = 0;
			chartArea1->AxisY->Title = L"Normalized Error";
			chartArea1->AxisY->TitleForeColor = System::Drawing::Color::White;
			chartArea1->AxisY2->LabelStyle->ForeColor = System::Drawing::Color::White;
			chartArea1->AxisY2->Minimum = 0;
			chartArea1->BackColor = System::Drawing::Color::Black;
			chartArea1->BorderColor = System::Drawing::Color::White;
			chartArea1->Name = L"ChartArea1";
			this->B_ERROR_FUNCTION->ChartAreas->Add(chartArea1);
			this->B_ERROR_FUNCTION->Location = System::Drawing::Point(3, 18);
			this->B_ERROR_FUNCTION->Name = L"B_ERROR_FUNCTION";
			series1->BorderWidth = 2;
			series1->ChartArea = L"ChartArea1";
			series1->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::FastLine;
			series1->Color = System::Drawing::Color::Yellow;
			series1->IsVisibleInLegend = false;
			series1->Name = L"error_function";
			this->B_ERROR_FUNCTION->Series->Add(series1);
			this->B_ERROR_FUNCTION->Size = System::Drawing::Size(340, 147);
			this->B_ERROR_FUNCTION->TabIndex = 28;
			this->B_ERROR_FUNCTION->Text = L"chart1";
			// 
			// tabControl1
			// 
			this->tabControl1->Controls->Add(this->Viewer);
			this->tabControl1->Controls->Add(this->tab_Scenario_generator);
			this->tabControl1->Location = System::Drawing::Point(608, 174);
			this->tabControl1->Name = L"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(349, 512);
			this->tabControl1->TabIndex = 29;
			// 
			// Viewer
			// 
			this->Viewer->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(128)), static_cast<System::Int32>(static_cast<System::Byte>(128)),
				static_cast<System::Int32>(static_cast<System::Byte>(255)));
			this->Viewer->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->Viewer->Controls->Add(this->groupBox11);
			this->Viewer->Controls->Add(this->groupBox8);
			this->Viewer->Controls->Add(this->groupBox7);
			this->Viewer->Controls->Add(this->groupBox6);
			this->Viewer->Location = System::Drawing::Point(4, 22);
			this->Viewer->Name = L"Viewer";
			this->Viewer->Padding = System::Windows::Forms::Padding(3);
			this->Viewer->Size = System::Drawing::Size(341, 486);
			this->Viewer->TabIndex = 0;
			this->Viewer->Text = L"Viewer";
			// 
			// groupBox8
			// 
			this->groupBox8->Controls->Add(this->B_LOAD_DETCTIONS);
			this->groupBox8->Controls->Add(this->B_DETECTIONS_FOLDER);
			this->groupBox8->ForeColor = System::Drawing::Color::White;
			this->groupBox8->Location = System::Drawing::Point(6, 228);
			this->groupBox8->Name = L"groupBox8";
			this->groupBox8->Size = System::Drawing::Size(317, 79);
			this->groupBox8->TabIndex = 3;
			this->groupBox8->TabStop = false;
			this->groupBox8->Text = L"Detections:";
			// 
			// B_LOAD_DETCTIONS
			// 
			this->B_LOAD_DETCTIONS->ForeColor = System::Drawing::Color::Blue;
			this->B_LOAD_DETCTIONS->Location = System::Drawing::Point(199, 45);
			this->B_LOAD_DETCTIONS->Name = L"B_LOAD_DETCTIONS";
			this->B_LOAD_DETCTIONS->Size = System::Drawing::Size(112, 31);
			this->B_LOAD_DETCTIONS->TabIndex = 1;
			this->B_LOAD_DETCTIONS->Text = L"Load";
			this->B_LOAD_DETCTIONS->UseVisualStyleBackColor = true;
			this->B_LOAD_DETCTIONS->Click += gcnew System::EventHandler(this, &MyForm::B_LOAD_DETCTIONS_Click);
			// 
			// B_DETECTIONS_FOLDER
			// 
			this->B_DETECTIONS_FOLDER->Location = System::Drawing::Point(6, 19);
			this->B_DETECTIONS_FOLDER->Name = L"B_DETECTIONS_FOLDER";
			this->B_DETECTIONS_FOLDER->Size = System::Drawing::Size(305, 20);
			this->B_DETECTIONS_FOLDER->TabIndex = 0;
			// 
			// groupBox7
			// 
			this->groupBox7->Controls->Add(this->B_LOAD_MAP_IMAGE);
			this->groupBox7->Controls->Add(this->B_FILE_MAP);
			this->groupBox7->ForeColor = System::Drawing::Color::White;
			this->groupBox7->Location = System::Drawing::Point(6, 313);
			this->groupBox7->Name = L"groupBox7";
			this->groupBox7->Size = System::Drawing::Size(317, 78);
			this->groupBox7->TabIndex = 2;
			this->groupBox7->TabStop = false;
			this->groupBox7->Text = L"Map image:";
			// 
			// B_LOAD_MAP_IMAGE
			// 
			this->B_LOAD_MAP_IMAGE->ForeColor = System::Drawing::Color::Blue;
			this->B_LOAD_MAP_IMAGE->Location = System::Drawing::Point(199, 45);
			this->B_LOAD_MAP_IMAGE->Name = L"B_LOAD_MAP_IMAGE";
			this->B_LOAD_MAP_IMAGE->Size = System::Drawing::Size(112, 31);
			this->B_LOAD_MAP_IMAGE->TabIndex = 5;
			this->B_LOAD_MAP_IMAGE->Text = L"Load";
			this->B_LOAD_MAP_IMAGE->UseVisualStyleBackColor = true;
			this->B_LOAD_MAP_IMAGE->Click += gcnew System::EventHandler(this, &MyForm::B_LOAD_MAP_IMAGE_Click);
			// 
			// B_FILE_MAP
			// 
			this->B_FILE_MAP->Location = System::Drawing::Point(6, 19);
			this->B_FILE_MAP->Name = L"B_FILE_MAP";
			this->B_FILE_MAP->Size = System::Drawing::Size(305, 20);
			this->B_FILE_MAP->TabIndex = 4;
			// 
			// groupBox6
			// 
			this->groupBox6->Controls->Add(this->B_LOAD_TOM);
			this->groupBox6->Controls->Add(this->B_FILE_TOM);
			this->groupBox6->ForeColor = System::Drawing::Color::White;
			this->groupBox6->Location = System::Drawing::Point(6, 397);
			this->groupBox6->Name = L"groupBox6";
			this->groupBox6->Size = System::Drawing::Size(317, 79);
			this->groupBox6->TabIndex = 1;
			this->groupBox6->TabStop = false;
			this->groupBox6->Text = L"True Obstacles-map (TOM):";
			// 
			// B_LOAD_TOM
			// 
			this->B_LOAD_TOM->ForeColor = System::Drawing::Color::Blue;
			this->B_LOAD_TOM->Location = System::Drawing::Point(199, 45);
			this->B_LOAD_TOM->Name = L"B_LOAD_TOM";
			this->B_LOAD_TOM->Size = System::Drawing::Size(112, 31);
			this->B_LOAD_TOM->TabIndex = 5;
			this->B_LOAD_TOM->Text = L"Load";
			this->B_LOAD_TOM->UseVisualStyleBackColor = true;
			this->B_LOAD_TOM->Click += gcnew System::EventHandler(this, &MyForm::B_BROWSE_OBSTACLES_FILE_Click);
			// 
			// B_FILE_TOM
			// 
			this->B_FILE_TOM->Location = System::Drawing::Point(6, 19);
			this->B_FILE_TOM->Name = L"B_FILE_TOM";
			this->B_FILE_TOM->Size = System::Drawing::Size(305, 20);
			this->B_FILE_TOM->TabIndex = 4;
			// 
			// tab_Scenario_generator
			// 
			this->tab_Scenario_generator->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(128)),
				static_cast<System::Int32>(static_cast<System::Byte>(128)), static_cast<System::Int32>(static_cast<System::Byte>(255)));
			this->tab_Scenario_generator->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->tab_Scenario_generator->Controls->Add(this->groupBox10);
			this->tab_Scenario_generator->Controls->Add(this->groupBox4);
			this->tab_Scenario_generator->Controls->Add(this->groupBox3);
			this->tab_Scenario_generator->Controls->Add(this->groupBox1);
			this->tab_Scenario_generator->Controls->Add(this->groupBox2);
			this->tab_Scenario_generator->ForeColor = System::Drawing::Color::Black;
			this->tab_Scenario_generator->Location = System::Drawing::Point(4, 22);
			this->tab_Scenario_generator->Name = L"tab_Scenario_generator";
			this->tab_Scenario_generator->Padding = System::Windows::Forms::Padding(3);
			this->tab_Scenario_generator->Size = System::Drawing::Size(341, 486);
			this->tab_Scenario_generator->TabIndex = 1;
			this->tab_Scenario_generator->Text = L"Scenario generator";
			// 
			// groupBox10
			// 
			this->groupBox10->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(128)), static_cast<System::Int32>(static_cast<System::Byte>(128)),
				static_cast<System::Int32>(static_cast<System::Byte>(255)));
			this->groupBox10->Controls->Add(this->B_SAVE_DETECTIONS_DIRECTORY);
			this->groupBox10->Controls->Add(this->B_EXPERIMENT_DIRECTORY);
			this->groupBox10->ForeColor = System::Drawing::Color::White;
			this->groupBox10->Location = System::Drawing::Point(2, 328);
			this->groupBox10->Name = L"groupBox10";
			this->groupBox10->Size = System::Drawing::Size(330, 77);
			this->groupBox10->TabIndex = 27;
			this->groupBox10->TabStop = false;
			this->groupBox10->Text = L"Experiment Directory:";
			// 
			// B_SAVE_DETECTIONS_DIRECTORY
			// 
			this->B_SAVE_DETECTIONS_DIRECTORY->ForeColor = System::Drawing::Color::Black;
			this->B_SAVE_DETECTIONS_DIRECTORY->Location = System::Drawing::Point(219, 44);
			this->B_SAVE_DETECTIONS_DIRECTORY->Name = L"B_SAVE_DETECTIONS_DIRECTORY";
			this->B_SAVE_DETECTIONS_DIRECTORY->Size = System::Drawing::Size(93, 27);
			this->B_SAVE_DETECTIONS_DIRECTORY->TabIndex = 7;
			this->B_SAVE_DETECTIONS_DIRECTORY->Text = L"Browse...";
			this->B_SAVE_DETECTIONS_DIRECTORY->UseVisualStyleBackColor = true;
			this->B_SAVE_DETECTIONS_DIRECTORY->Click += gcnew System::EventHandler(this, &MyForm::B_SAVE_DETECTIONS_DIRECTORY_Click);
			// 
			// B_EXPERIMENT_DIRECTORY
			// 
			this->B_EXPERIMENT_DIRECTORY->Location = System::Drawing::Point(16, 18);
			this->B_EXPERIMENT_DIRECTORY->Name = L"B_EXPERIMENT_DIRECTORY";
			this->B_EXPERIMENT_DIRECTORY->Size = System::Drawing::Size(296, 20);
			this->B_EXPERIMENT_DIRECTORY->TabIndex = 6;
			this->B_EXPERIMENT_DIRECTORY->TextChanged += gcnew System::EventHandler(this, &MyForm::B_FILE_DETECTIONS_GENERATOR_TextChanged);
			// 
			// groupBox9
			// 
			this->groupBox9->Controls->Add(this->B_ERROR_FUNCTION);
			this->groupBox9->ForeColor = System::Drawing::Color::White;
			this->groupBox9->Location = System::Drawing::Point(608, 0);
			this->groupBox9->Name = L"groupBox9";
			this->groupBox9->Size = System::Drawing::Size(349, 171);
			this->groupBox9->TabIndex = 29;
			this->groupBox9->TabStop = false;
			this->groupBox9->Text = L"Error function";
			// 
			// B_BROWSE_DIR
			// 
			this->B_BROWSE_DIR->RootFolder = System::Environment::SpecialFolder::MyDocuments;
			// 
			// groupBox11
			// 
			this->groupBox11->Controls->Add(this->B_VIEWER_STEP);
			this->groupBox11->Controls->Add(this->B_VIEWER_PLAY);
			this->groupBox11->ForeColor = System::Drawing::Color::White;
			this->groupBox11->Location = System::Drawing::Point(8, 144);
			this->groupBox11->Name = L"groupBox11";
			this->groupBox11->Size = System::Drawing::Size(314, 84);
			this->groupBox11->TabIndex = 4;
			this->groupBox11->TabStop = false;
			this->groupBox11->Text = L"Viewer";
			// 
			// B_VIEWER_STEP
			// 
			this->B_VIEWER_STEP->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->B_VIEWER_STEP->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->B_VIEWER_STEP->ForeColor = System::Drawing::Color::Black;
			this->B_VIEWER_STEP->Location = System::Drawing::Point(91, 19);
			this->B_VIEWER_STEP->Name = L"B_VIEWER_STEP";
			this->B_VIEWER_STEP->Size = System::Drawing::Size(73, 48);
			this->B_VIEWER_STEP->TabIndex = 8;
			this->B_VIEWER_STEP->Text = L"Step";
			this->B_VIEWER_STEP->UseVisualStyleBackColor = true;
			this->B_VIEWER_STEP->Click += gcnew System::EventHandler(this, &MyForm::B_VIEWER_STEP_Click);
			// 
			// B_VIEWER_PLAY
			// 
			this->B_VIEWER_PLAY->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->B_VIEWER_PLAY->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->B_VIEWER_PLAY->ForeColor = System::Drawing::Color::Black;
			this->B_VIEWER_PLAY->Location = System::Drawing::Point(6, 19);
			this->B_VIEWER_PLAY->Name = L"B_VIEWER_PLAY";
			this->B_VIEWER_PLAY->Size = System::Drawing::Size(69, 48);
			this->B_VIEWER_PLAY->TabIndex = 7;
			this->B_VIEWER_PLAY->Text = L"Play";
			this->B_VIEWER_PLAY->UseVisualStyleBackColor = true;
			this->B_VIEWER_PLAY->Click += gcnew System::EventHandler(this, &MyForm::B_VIEWER_PLAY_Click);
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::Color::Blue;
			this->ClientSize = System::Drawing::Size(960, 690);
			this->Controls->Add(this->groupBox9);
			this->Controls->Add(this->tabControl1);
			this->Controls->Add(this->B_PANEL);
			this->Controls->Add(this->groupBox5);
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
			this->Name = L"MyForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"OMR Ver. 2.0";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &MyForm::MyForm_FormClosing);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->B_HELICOPTER_SPEED))->EndInit();
			this->groupBox2->ResumeLayout(false);
			this->groupBox2->PerformLayout();
			this->groupBox3->ResumeLayout(false);
			this->groupBox3->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->B_WIRE_SEGMENT_LENGTH_METER))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->B_RELIABILITY_THRESHOLD))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->B_MAX_ERROR_RANGE))->EndInit();
			this->groupBox4->ResumeLayout(false);
			this->groupBox4->PerformLayout();
			this->panel1->ResumeLayout(false);
			this->panel1->PerformLayout();
			this->B_PANEL->ResumeLayout(false);
			this->B_PANEL->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->B_WIRE_VARIANCE))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->B_FALSE_ALARM_RATE))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->B_RADAR_REFRESH))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->B_RADAR_FOV))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->B_RADAR_RANGE))->EndInit();
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->groupBox5->ResumeLayout(false);
			this->groupBox5->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->B_SCREEN_WIDTH))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->B_ERROR_FUNCTION))->EndInit();
			this->tabControl1->ResumeLayout(false);
			this->Viewer->ResumeLayout(false);
			this->groupBox8->ResumeLayout(false);
			this->groupBox8->PerformLayout();
			this->groupBox7->ResumeLayout(false);
			this->groupBox7->PerformLayout();
			this->groupBox6->ResumeLayout(false);
			this->groupBox6->PerformLayout();
			this->tab_Scenario_generator->ResumeLayout(false);
			this->groupBox10->ResumeLayout(false);
			this->groupBox10->PerformLayout();
			this->groupBox9->ResumeLayout(false);
			this->groupBox11->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion
	private:
		//--------------------------------------------------------
		//    Variables
		//--------------------------------------------------------
		T_INITIAL_data*		INITIAL_data;
		T_OBSTACLES_map		OBSTACLES_map_true;
		T_OBSTACLES_map		OBSTACLES_map_estimated;
		T_HELICOPTER_route	HELICOPTER_route_master;
		T_HELICOPTER_route	HELICOPTER_route;
		fstream				*Recording_file_handle;
		bool				Experiment_initialized;
		bool				Viewer_initialized;
		array<T_Target>^	Radar_detections_array;
		int					Radar_detections_counter;
		array<double>^		Error_function_array;
		int					Error_function_old_counter;
		bool				Start_wire_from_mouse;
		PointF				Last_screen_point_of_wire;
		PointF				Point_mouse_move;

		System::Collections::Generic::List<PointD>^ helicopter_routing;

		//--------------------------------------------------------
		//    Methods
		//--------------------------------------------------------
		PointF		convert_UTM_to_Screen(PointD UTM_coordinates);
		PointD		convert_Screen_to_UTM(PointF Screen_coordinates);
		PointD		convert_Spherical_to_Cartesian(PointD Spherical_point);
		PointD		convert_Cartesian_to_Spherical(PointD Cartesian_point);
		//PointF		convert_Radar_to_Screen(PointD Radar_coordinates);
		double		Distance_between_points(PointD p1, PointD p2);
		double		Distance_between_point_and_line(PointD P, PointD L1, PointD L2);
		double		Line_angle(PointD p1, PointD p2, PointD p3);
		PointD 		Line_point_projection(PointD p1, PointD p2, PointD p3);
		int 		Target_is_in_RADAR_FOV(PointD Radar, float Radar_angle, PointD Target);
		void		My_message(String^ message_string);
		void		Init_screen();

		void		Obstacles_map_estimate();
		void		Obstacles_map_plot(T_OBSTACLES_map obstacles_map, Color pen_Color);
		void		Obstacles_map_error_function();
		void		Obstacles_map_load(T_OBSTACLES_map % obstacles_map, char* file_name);

		int			Experiment_init();
		void		Experiment_finalize();
		void		Experiment_radar_operation();
		int			Experiment_create_folder(char* experiment_path);
		int			Experiment_Save_def_file(char*	experiment_path);

		int			Viewer_init();
		int			Viewer_finalize();
		int			Viewer_radar_operation();
		int			Viewer_calculate_experiment_area();

		void		B_PANEL_MouseClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
		void		B_CLEAR_WIRE_Click(System::Object^  sender, System::EventArgs^  e);
		void		MyForm_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e);
		void		B_PANEL_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e);
		void		B_PANEL_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
		void		B_PANEL_MouseLeave(System::Object^  sender, System::EventArgs^  e);
		void		B_FLY_Click(System::Object^  sender, System::EventArgs^  e);
		void		B_VIEWER_PLAY_Click(System::Object^  sender, System::EventArgs^  e);
		void		B_VIEWER_STEP_Click(System::Object^  sender, System::EventArgs^  e);
		void		B_SPEED_ValueChanged(System::Object^  sender, System::EventArgs^  e);
		void		B_WIRE_VARIANCE_ValueChanged(System::Object^  sender, System::EventArgs^  e);
		void		B_FALSE_ALARM_RATE_ValueChanged(System::Object^  sender, System::EventArgs^  e);
		void		B_RADAR_RANGE_ValueChanged(System::Object^  sender, System::EventArgs^  e);
		void		B_RADAR_FOV_ValueChanged(System::Object^  sender, System::EventArgs^  e);
		void		B_RADAR_REFRESH_ValueChanged(System::Object^  sender, System::EventArgs^  e);
		void		B_SCREEN_WIDTH_Scroll(System::Object^  sender, System::EventArgs^  e);
		void		B_ROUTE_NEW_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		void		B_SAVE_OBSTACLES_FILE_Click(System::Object^  sender, System::EventArgs^  e);
		void		B_SAVE_DETECTIONS_DIRECTORY_Click(System::Object^  sender, System::EventArgs^  e);
		void		B_BROWSE_OBSTACLES_FILE_Click(System::Object^  sender, System::EventArgs^  e);
		void		B_STEP_FORWARD_Click(System::Object^  sender, System::EventArgs^  e);
		void		B_PLOT_DETECTIONS_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		void		B_RELIABILITY_THRESHOLD_ValueChanged(System::Object^  sender, System::EventArgs^  e);
		void		B_MAX_ERROR_RANGE_ValueChanged(System::Object^  sender, System::EventArgs^  e);
		void		B_WIRE_SEGMENT_LENGTH_METER_ValueChanged(System::Object^  sender, System::EventArgs^  e);
		void		B_GPS_LAT_TextChanged(System::Object^  sender, System::EventArgs^  e);
		void		B_GPS_LONG_TextChanged(System::Object^  sender, System::EventArgs^  e);
		void		B_FILE_DETECTIONS_GENERATOR_TextChanged(System::Object^  sender, System::EventArgs^  e);
		void		B_LOAD_DETCTIONS_Click(System::Object^  sender, System::EventArgs^  e);
		void		B_LOAD_MAP_IMAGE_Click(System::Object^  sender, System::EventArgs^  e);
		// Added by Cogniteam
		void		GroundTruthToObj(System::Collections::Generic::List<PointD>^ helicopter_route);




	};
}
