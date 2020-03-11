/*main.cpp*/

//
// Ria Gupta
// Project: GradeUtil UI
// This program is C++11 dependent
//

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
using namespace std;

// includes for gradeutil
#include "gradeutil.h"

College InputGradeData(string filename)
{
    College college;
    ifstream file(filename);
    string line, value;
    int totalcourses=0;
    int dfw=0;
    int tnum=0;
    int totalstudents=0;
  
    if (!file.good())
    {
        cout << "**Error: unable to open input file '" << filename << "'." << endl;
        return college;
    }

    // first line contains semester,year
    getline(file, line);
    stringstream ss(line);
    getline(ss, college.Name, ',');
    getline(ss, college.Semester, ',');
    getline(ss, value);
    college.Year = stoi(value);
    getline(file, line); // second line contains column headers --- skip

    //
    // now start inputting and parse course data:
    //

    while (getline(file, line))
    {
        Course c = ParseCourse(line);
        totalstudents = totalstudents + c.getNumStudents();
        //
        // search for correct dept to ask course to, otherwise create a new dept:
        //
        auto dept_iter = std::find_if(college.Depts.begin(),
                                      college.Depts.end(),
                                      [&](const Dept &d) {
                                          return (d.Name == c.Dept);
                                      });

        if (dept_iter == college.Depts.end())
        {
            // doesn't exist, so we have to create a new dept and insert course
            Dept d(c.Dept);
            d.Courses.push_back(c);
            college.Depts.push_back(d);
        }
        else
        {
            // dept exists, so insert course into existing dept:
            dept_iter->Courses.push_back(c);
        }

    } //while
  cout << "** College of "<< college.Name<<", "<<college.Semester<<" "<<college.Year<<" **"<<endl;
  
  for(auto &x : college.Depts)
  {
    totalcourses += x.Courses.size();
  }
 
    sort(college.Depts.begin(), college.Depts.end(),
      [&](const Dept &one, const Dept &two)
       {
         if(one.Name < two.Name)
         {return true;}
         else {return false;}
       }
      );
  
  GradeStats grade= GetGradeDistribution(college);
  double DFWRate= GetDFWRate(college, tnum, dfw);
  
  cout<<"# of courses taught: "<< totalcourses<<endl;
  cout<<"# of students taught: "<< totalstudents<<endl;
  cout<<"grade distribution (A-F): "<<grade.PercentA<<"%, "<<grade.PercentB<<"%, "<<grade.PercentC<<"%, "<<grade.PercentD<<"%, "<<grade.PercentF<<"%"<<endl;
  cout<<"DFW rate: "<<DFWRate<<"%"<<endl;
  
    return college;
}

Dept sendDept (const College &c, string input)
{
  Dept dp;
      auto iter = find_if(c.Depts.begin(), c.Depts.end(),
                       [&](const Dept &d)
                        {
                          if(input == d.Name)
                          {
                            dp = d;
                            return true;
                          }
                          else 
                          {
                            return false;
                          }
                        }
                       );
  if(iter == c.Depts.end())
  {
    Dept d("empty");
    return d;
  }
  else 
  {
    return dp;
  }
  //return d;
}

void sortCourses(Dept &x)
{
    sort(x.Courses.begin(), x.Courses.end(),
         [&](const Course &one, const Course &two)
         {
            if (one.Number < two.Number) return true;
               //return true;
            else if (one.Number > two.Number) return false;
               //return false;
            else if (one.Section < two.Section) return true;
              //if (one.Section < two.Section)
                // return true;
              else 
                 return false;
		     });
}

void sortDept(College &c)
{
  sort(c.Depts.begin(), c.Depts.end(),
      [&](const Dept &one, const Dept &two)
       {
         if(one.Name < two.Name)
         {
           return true;
         }
         else 
         {
           return false;
         }
       });
  for(auto &y: c.Depts)
  {
    sortCourses(y);
  }
}

void sendSummary(const Dept &d)
{
  int dfw = 0;
  int num = 0;
  int studentcount = 0;
  GradeStats gs = GetGradeDistribution(d);
  
  cout<< d.Name << ":" << endl;
  cout << " # courses taught: " << d.Courses.size()<< endl;
  
  for (const Course &z: d.Courses)
  {
    studentcount += z.getNumStudents();
  }
  
  cout << " # students taught: " << studentcount << endl;
  cout << " grade distribution (A-F): " << gs.PercentA <<"%, " <<
    gs.PercentB <<"%, "<< gs.PercentC <<"%, "<< gs.PercentD <<"%, "
    << gs.PercentF <<"% " << endl;
  cout << " DFW rate: " << GetDFWRate(d, dfw, num) << "%" << endl;
  
}

void summary(const College& c)
{
  string input;
  cout << "dept name, or all? ";
  cin >> input;
  
  if(input =="all")
  {
    for(const Dept& d: c.Depts)
    {
      sendSummary(d);
    }
  }
  else
  {
    Dept x = sendDept(c, input);
      if(x.Name != "empty")
      {
        sendSummary(x);
      }
    else 
    {
      cout << "**dept not found" << endl;
      return;
    }
//     auto iter = find_if(c.Depts.begin(), c.Depts.end(),
//                         [&](const Dept &d)
//                         {
//                           if(input == d.Name)
//                           {return true;}
//                           else {return false;}
//                         }
//                        );
//     if(iter == c.Depts.end())
//     {
//       cout << "**dept not found" << endl;
//       return;
//     }
//     else
//     {
//       auto x = *iter;
//       sendSummary(x);
//     }
  }
  
}// end of summary

void sendCourseinfo(const vector<Course> &course)
{ 
  for (auto& c: course)
  {
    int dfw = 0;
    int num = 0;
    GradeStats gs = GetGradeDistribution(c);
    string courseType;
    if(c.getGradingType()== 0)
      {courseType ="letter";}
    else if(c.getGradingType()== 1)
      {courseType = "satisfactory";}
    else if(c.getGradingType()== 2)
      {courseType = "unknown";}

    cout << c.Dept << " " << c.Number << " (section " << c.Section << "): " 
      << c.Instructor<< endl;
    cout << " # students: "<< c.getNumStudents() << endl;
    cout << " course type: "<< courseType << endl;
    cout << " grade distribution (A-F): " << gs.PercentA <<"%, " <<
      gs.PercentB <<"%, "<< gs.PercentC <<"%, "<< gs.PercentD <<"%, "
      << gs.PercentF <<"% " << endl;
    cout << " DFW rate: " << GetDFWRate(c, dfw, num) << "%" << endl;
  }
 
}

void search(const College &c)
{
  string input;
  string instructorPrefix;
  int courseNum;
  vector<Course> x;
  
  cout << "dept name, or all? ";
  cin >> input;
  cout << "course # or instructor prefix? ";
  cin >> instructorPrefix;
  stringstream ss(instructorPrefix);
  ss >> courseNum;
  
  if(input == "all" && ss.fail())
  {  
      x = FindCourses(c, instructorPrefix);
      if(x.size()== 0)
      {
        cout << "**none found" << endl;
        return;
      }
      sendCourseinfo(x);
  }    
  else if(input == "all")
  {
    x = FindCourses(c, courseNum);
    if(x.size()==0)
    {
      cout << "**none found" << endl;
      return;
    }
    sendCourseinfo(x);
  }
  else if(ss.fail())
  {
     auto iter = find_if(c.Depts.begin(), c.Depts.end(),
                     [&](const Dept &d)
                      {
                        if(input == d.Name)
                          return true;
                        else
                          return false;
                      }
                     );
    if (iter == c.Depts.end())
    {
      cout<<"**dept not found"<<endl;
      return;
    }
    auto y = (*iter);
    x = FindCourses(y, instructorPrefix);
    if(x.size() == 0)
    {
      cout << "**none found" << endl;
      return;
    }
    sendCourseinfo(x);
  }

  else 
  {
    auto iter = find_if(c.Depts.begin(), c.Depts.end(),
                       [&](const Dept &d)
                        {
                          if (d.Name == input)
                          {return true;}
                          else
                          {return false;}
                        }
                       );
    if (iter == c.Depts.end())
    {
      cout << "**dept not found" << endl;
      return;
    }
    x = FindCourses(*iter, courseNum);
      if(x.size()==0)
      {
        cout << "**none found" << endl;
        return;
      }
      sendCourseinfo(x);
    
    return;
  }
 }// end of search

void satisfactory(const College &c)
{
  string input;
  cout << "dept name, or all? ";
  cin >> input;
  //vector<Course> storecourse;
  int count = 0;
  if( input =="all")
  {
    for(const Dept &x: c.Depts)
    {
      for(const Course &y: x.Courses)
      {
        if(y.getGradingType()== 1)
        {
          count++;
          //storecourse.push_back(y);
          //sendCourseinfo(storecourse);
          string courseType;
          if(y.getGradingType()== 0)
            {courseType ="letter";}
          else if(y.getGradingType()== 1)
            {courseType = "satisfactory";}
          else if(y.getGradingType()== 2)
            {courseType = "unknown";}

          cout << y.Dept << " " << y.Number << " (section " << y.Section << "): " 
            << y.Instructor<< endl;
          cout << " # students: "<< y.getNumStudents() << endl;
          cout << " course type: "<< courseType << endl;
        }
      }
    }
  }
  else 
  {
    auto iter = find_if(c.Depts.begin(), c.Depts.end(),
                       [&](const Dept &d)
                        {
                          if(input == d.Name)
                          {return true;}
                          else 
                          {return false;}
                        }
                       );
    if(iter == c.Depts.end())
    {
      cout << "**dept not found"<< endl;
      return;
    }
    else
    {
      for(auto &y: (*iter).Courses)
      {
        if(y.getGradingType()== 1)
        {
          count++;

          string courseType;
          if(y.getGradingType()== 0)
            {courseType ="letter";}
          else if(y.getGradingType()== 1)
            {courseType = "satisfactory";}
          else if(y.getGradingType()== 2)
            {courseType = "unknown";}

          cout << y.Dept << " " << y.Number << " (section " << y.Section << "): " 
            << y.Instructor<< endl;
          cout << " # students: "<< y.getNumStudents() << endl;
          cout << " course type: "<< courseType << endl;
        }
      }
    }
  }
  if(count== 0)
  {
    cout << "**none found" << endl;
    return;
  }
}// end of satisfactory

vector<Course> sortdfw(vector<Course> &course)
{
  int dfw, n = 0;
  sort(course.begin(), course.end(),
      [&](const Course &one, const Course &two)
       {
         if(GetDFWRate(one, dfw,n) > GetDFWRate(two, dfw, n))
         {
           return true;
         }
         else
         {
           return false;
         }
       }
      );
  return course;
}

void dfw(const College &c)
{
  string input;
  double threshold = 0.0;
  int dfw = 0;
  int n = 0;
  vector<Course> sorteddfw;
  
  cout << "dept name, or all? ";
  cin >> input;
  cout << "dfw threshold? ";
  cin >> threshold;
  
  if(input == "all")
  {
    for(auto& x: c.Depts)
    {
       for(auto& y: x.Courses)
      {
        if(GetDFWRate(y, dfw, n) > threshold)
        {
          sorteddfw.push_back(y);
        }
      }
    }
  }
  else 
  {
    auto iter = find_if(c.Depts.begin(), c.Depts.end(),
                       [&](const Dept &d)
                        {
                          if(input == d.Name)
                          {return true;}
                          else 
                          {return false;}
                        }
                       );
    if(iter == c.Depts.end())
    {
      cout << "**dept not found" <<endl;
      return;
    }
    else 
    {
      for(auto &x: (*iter).Courses)
      {
        if(GetDFWRate(x, dfw, n) > threshold)
        {
          sorteddfw.push_back(x);
        }
      }
    }
  }
  if(sorteddfw.size() == 0)
  {
    cout << "**none found " << endl;
    return;
  }
  vector<Course> sorted_dfw = sortdfw(sorteddfw);
  sendCourseinfo(sorted_dfw);

}// end of dfw

vector<Course> sendsortedB(vector<Course> &c)
{
  sort(c.begin(), c.end(),
      [&](const Course &one, const Course &two)
       {
         if(GetGradeDistribution(one).PercentB > GetGradeDistribution(two).PercentB)
         {
           return true;
         }
         else if(GetGradeDistribution(one).PercentB == GetGradeDistribution(two).PercentB)
         {
            if (one.Dept < two.Dept)
              return true;
            else if (one.Dept == two.Dept)
            {
              if (one.Number < two.Number)
              {
                return true;
              }
              else if(one.Number == two.Number)
              {
                if (one.Section < two.Section)
                {
                  return true;
                }
                else 
                {
                  return false;
                }
              }
              else 
              {
                return false;
              }
            }
            else
            {
              return false;
            }
         }
         else 
         {
           return false;
         }
       });
  return c;
}

void letterB(const College &c)
{
  string input;
  double threshold = 0.0;
  vector<Course> letterb;
  
  cout << "dept name, or all? ";
  cin >> input;
  cout << "letter B threshold? ";
  cin >> threshold;
  
  if(input =="all")
  {
    for(auto &x: c.Depts)
    {
      for(auto &y: x.Courses)
      {
        if(GetGradeDistribution(y).PercentB > threshold)
        {
          letterb.push_back(y);
        }
      }
    }
  }
  else 
  {
    auto iter = find_if(c.Depts.begin(), c.Depts.end(),
                        [&](const Dept &d)
                        {
                          if(input == d.Name)
                          {
                            return true;
                          }
                          else 
                          {
                            return false;
                          }
                        });
    if(iter == c.Depts.end())
    {
      cout<< "**dept not found" << endl;
      return;
    }
    else 
    {
      for(auto &x: (*iter).Courses)
      {
        if(GetGradeDistribution(x).PercentB > threshold)
        {
          letterb.push_back(x);
        }
      }
    }
  }
  if(letterb.size() == 0)
  {
    cout << "**none found" << endl;
    return;
  }
  
  vector<Course> sortedletterb = sendsortedB(letterb);
  sendCourseinfo(sortedletterb);
}

void average(const College &c)
{
  string input;
  cout << "dept name, or all? ";
  cin >> input;
  vector<Course> avgcourse;
  vector<pair<double, string>> dp; 
  double avgdept= 0.0;
  double avgclass= 0.0;
  int totalcourse = 0;
  
  if(input == "all")
  {
    for(auto &x: c.Depts)
    {
      for(auto &y: x.Courses)
      {
        if(y.getGradingType()== 0)
        {
          avgcourse.push_back(y);
        }
      }
    }
    for(const Dept&d : c.Depts)
    {
      for(const Course&z: d.Courses)
      {
        if(z.getGradingType()== Course::Letter)
        {
          GradeStats gs = GetGradeDistribution(z);
           double total = 4.0*gs.NumA;
           total += 3.0*gs.NumB;
           total += 2.0*gs.NumC;
           total += 1.0*gs.NumD;
          total += 0.0*gs.NumF;
            double totalN = gs.N;
          double gpa =(double)total/(double)totalN;
          if(totalN == 0)
          { gpa = 0.0;}
          avgclass += gpa;
          totalcourse++;
        }
      }
        avgdept= (double)avgclass/(double)totalcourse;
     
      auto kvpair= pair<double, string>(avgdept, d.Name);
      dp.push_back(kvpair);
      
    }
    sort(dp.begin(), dp.end(), 
        [&](const pair<double, string> &one, const pair<double, string> &two)
         {
           if(one.first > two.first)
           {return true;}
           else {return false;}
         });
    for(auto iter = dp.begin(); iter !=dp.end(); ++iter)
    {
      cout <<"Overall GPA for "<< iter->second <<" : " <<iter->first<<endl;
    }
  }
}//end of average


int main()
{
    string filename;
    cout << std::fixed;
    cout << std::setprecision(2);

    cin >> filename; //1. Input the filename and then the grade data:
    College college = InputGradeData(filename);
    cout<<"\n";
    string command;
  sortDept(college);
  
  cout << "Enter a command> ";
  cin>> command;
  
  while(command !="#")
  {
    if(command =="summary")
    {
      summary(college);
    }
    else if (command =="search")
    {
      search(college);
    }
    else if (command =="satisfactory")
    {
      satisfactory(college);
    }
    else if(command =="dfw")
    {
      dfw(college);
    }
    else if(command =="letterB")
    {
      letterB(college);
    }
    else if(command =="average")
    {
      average(college);
    }
    else
    {
      cout<<"**unknown command"<<endl;
    }
    cout <<"Enter a command> ";
    cin >>command;
  }
  
}