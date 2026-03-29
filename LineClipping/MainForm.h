#pragma once

namespace LineClipping {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Collections::Generic;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public ref class MainForm : public System::Windows::Forms::Form
	{
	public:
		MainForm(void)
		{
			InitializeComponent();

			this->Text = L"Line Clipping";
			this->BackColor = System::Drawing::Color::White;
			this->Width = 1200;
			this->Height = 450;

			lines = gcnew List<Line>();

			lines->Add(Line(120, 120, 250, 200)); // inside
			lines->Add(Line(50, 50, 350, 280));   // partially outside
			lines->Add(Line(80, 150, 200, 150));  // left outside
			lines->Add(Line(150, 50, 150, 300));  // top-bottom outside
			lines->Add(Line(320, 120, 380, 220)); // fully outside

			this->Paint += gcnew PaintEventHandler(this, &MainForm::MainForm_Paint);
		}

	protected:
		~MainForm()
		{
			if (components)
			{
				delete components;
			}
		}

	private:
		value struct Line
		{
			double x1, y1, x2, y2;

			Line(double ax1, double ay1, double ax2, double ay2)
			{
				x1 = ax1;
				y1 = ay1;
				x2 = ax2;
				y2 = ay2;
			}
		};

		List<Line>^ lines;

		literal int INSIDE = 0;
		literal int LEFT = 1;
		literal int RIGHT = 2;
		literal int BOTTOM = 4;
		literal int TOP = 8;

		double x_min = 100;
		double y_min = 100;
		double x_max = 300;
		double y_max = 250;

		int ComputeCode(double x, double y)
		{
			int code = INSIDE;

			if (x < x_min) code |= LEFT;
			else if (x > x_max) code |= RIGHT;

			if (y < y_min) code |= TOP;
			else if (y > y_max) code |= BOTTOM;

			return code;
		}

		bool CohenSutherlandClip(double% x1, double% y1, double% x2, double% y2)
		{
			int code1 = ComputeCode(x1, y1);
			int code2 = ComputeCode(x2, y2);

			bool accept = false;

			while (true)
			{
				if ((code1 == 0) && (code2 == 0))
				{
					accept = true;
					break;
				}
				else if ((code1 & code2) != 0)
				{
					break;
				}
				else
				{
					double x = 0.0, y = 0.0;
					int code_out = (code1 != 0) ? code1 : code2;

					if ((code_out & TOP) != 0)
					{
						x = x1 + (x2 - x1) * (y_min - y1) / (y2 - y1);
						y = y_min;
					}
					else if ((code_out & BOTTOM) != 0)
					{
						x = x1 + (x2 - x1) * (y_max - y1) / (y2 - y1);
						y = y_max;
					}
					else if ((code_out & RIGHT) != 0)
					{
						y = y1 + (y2 - y1) * (x_max - x1) / (x2 - x1);
						x = x_max;
					}
					else if ((code_out & LEFT) != 0)
					{
						y = y1 + (y2 - y1) * (x_min - x1) / (x2 - x1);
						x = x_min;
					}

					if (code_out == code1)
					{
						x1 = x;
						y1 = y;
						code1 = ComputeCode(x1, y1);
					}
					else
					{
						x2 = x;
						y2 = y;
						code2 = ComputeCode(x2, y2);
					}
				}
			}

			return accept;
		}

		bool LiangBarskyClip(double% x1, double% y1, double% x2, double% y2)
		{
			double dx = x2 - x1;
			double dy = y2 - y1;

			array<double>^ p = { -dx, dx, -dy, dy };
			array<double>^ q = { x1 - x_min, x_max - x1, y1 - y_min, y_max - y1 };

			double u1 = 0.0;
			double u2 = 1.0;

			for (int i = 0; i < 4; i++)
			{
				if (p[i] == 0)
				{
					if (q[i] < 0)
						return false;
				}
				else
				{
					double u = q[i] / p[i];

					if (p[i] < 0)
					{
						if (u > u1) u1 = u;
					}
					else
					{
						if (u < u2) u2 = u;
					}
				}
			}

			if (u1 > u2)
				return false;

			double nx1 = x1 + u1 * dx;
			double ny1 = y1 + u1 * dy;
			double nx2 = x1 + u2 * dx;
			double ny2 = y1 + u2 * dy;

			x1 = nx1;
			y1 = ny1;
			x2 = nx2;
			y2 = ny2;

			return true;
		}

		void DrawWindow(Graphics^ g, int offsetX, String^ title)
		{
			System::Drawing::Pen^ blackPen = gcnew System::Drawing::Pen(System::Drawing::Color::Black, 2);
			System::Drawing::Font^ drawFont = gcnew System::Drawing::Font(L"Arial", 10);
			System::Drawing::Brush^ brush = System::Drawing::Brushes::Black;

			g->DrawString(title, drawFont, brush, (float)(offsetX + 120), 50.0f);
			g->DrawRectangle(
				blackPen,
				offsetX + (int)x_min,
				(int)y_min,
				(int)(x_max - x_min),
				(int)(y_max - y_min)
			);
		}

		void DrawOriginal(Graphics^ g, int offsetX)
		{
			System::Drawing::Pen^ bluePen = gcnew System::Drawing::Pen(System::Drawing::Color::Blue, 2);

			for each (Line line in lines)
			{
				g->DrawLine(
					bluePen,
					(float)(line.x1 + offsetX), (float)line.y1,
					(float)(line.x2 + offsetX), (float)line.y2
				);
			}
		}

		void DrawCohen(Graphics^ g, int offsetX)
		{
			Pen^ grayPen = gcnew Pen(Color::LightGray, 1);
			Pen^ redPen = gcnew Pen(Color::Red, 2);

			for each (Line line in lines)
			{
				g->DrawLine(
					grayPen,
					(float)(line.x1 + offsetX), (float)line.y1,
					(float)(line.x2 + offsetX), (float)line.y2
				);

				double x1 = line.x1;
				double y1 = line.y1;
				double x2 = line.x2;
				double y2 = line.y2;

				if (CohenSutherlandClip(x1, y1, x2, y2))
				{
					g->DrawLine(
						redPen,
						(float)(x1 + offsetX), (float)y1,
						(float)(x2 + offsetX), (float)y2
					);
				}
			}
		}

		void DrawLiang(Graphics^ g, int offsetX)
		{
			Pen^ grayPen = gcnew Pen(Color::LightGray, 1);
			Pen^ greenPen = gcnew Pen(Color::Green, 2);

			for each (Line line in lines)
			{
				g->DrawLine(
					grayPen,
					(float)(line.x1 + offsetX), (float)line.y1,
					(float)(line.x2 + offsetX), (float)line.y2
				);

				double x1 = line.x1;
				double y1 = line.y1;
				double x2 = line.x2;
				double y2 = line.y2;

				if (LiangBarskyClip(x1, y1, x2, y2))
				{
					g->DrawLine(
						greenPen,
						(float)(x1 + offsetX), (float)y1,
						(float)(x2 + offsetX), (float)y2
					);
				}
			}
		}

		System::Void MainForm_Paint(System::Object^ sender, System::Windows::Forms::PaintEventArgs^ e)
		{
			Graphics^ g = e->Graphics;

			DrawWindow(g, 0, L"Original Lines");
			DrawOriginal(g, 0);

			DrawWindow(g, 350, L"Cohen-Sutherland");
			DrawCohen(g, 350);

			DrawWindow(g, 700, L"Liang-Barsky");
			DrawLiang(g, 700);
		}

	private:
		System::ComponentModel::Container^ components;

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->components = gcnew System::ComponentModel::Container();
			this->Size = System::Drawing::Size(300, 300);
			this->Text = L"MainForm";
			this->Padding = System::Windows::Forms::Padding(0);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
		}
#pragma endregion
	};
}