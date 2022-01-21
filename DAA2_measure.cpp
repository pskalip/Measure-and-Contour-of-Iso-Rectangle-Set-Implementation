#include <bits/stdc++.h>

using namespace std ;

///Rectangle Structure
typedef struct rectangle
{
    pair<long double, long double> x_int ; //x_interval(bottpm, top)
    pair<long double, long double> y_int ; //y_interval(bottom, top)
} rectangle ;

///Edge (left or right) Structure
typedef struct edge
{
    pair<long double, long double> y_int ;
    long double x ;
    int edgetype ; // 0 - left, 1 - right
} edge ;

///Comparator for which edge comes first
bool comparator_edge(const edge& lhs, const edge& rhs) 
{
    if(lhs.x==rhs.x)
        return lhs.edgetype < rhs.edgetype;
   return lhs.x < rhs.x ;
}

///structure for stripe which contains x and y intervals
typedef struct stripe
{
    pair<long double, long double> x_int ;
    pair<long double, long double> y_int ;
    long double x_measure ;
} stripe ;

///structure definition for return type of stripe functions
typedef struct ret_stripe
{
    vector<pair<long double, long double>> L ;
    vector<pair<long double, long double>> R ;
    vector<long double> P ;
    vector<stripe> S ;
} ret_stripe ;

///generates partition for list of points
vector<pair<long double,long double>> partition(vector<long double> Y)
{
    sort(Y.begin(), Y.end()) ;
    vector<pair<long double,long double>> ans ;

    for(int i = 0; i < Y.size() - 1; i++)
    {
        pair<long double,long double> temp ;
        temp.first = Y[i] ;
        temp.second = Y[i + 1] ;
        ans.push_back(temp) ;
    }
    return ans ;
}

/// checks if interval is present in set of intervals
bool is_present(pair<long double, long double> p, vector<pair<long double, long double>> vec)
{
    for(int i = 0; i < vec.size(); i++)
    {
        if((p.first == vec[i].first) && (p.second == vec[i].second))
            return true ;
    }
    return false ;
}

/// check if point is present
bool is_present_int(long double p, vector<long double> vec)
{
    for(int i = 0; i < vec.size(); i++)
    {
        if(p == vec[i])
            return true ;
    }
    return false ;
}

/// check whether interval is superset of another one
int superset(pair<long double, long double> p1, pair<long double, long double> p2)
{
	if(p1.first <= p2.first && p1.second >= p2.second) 
		return 1 ;
	return 0 ;
}

int subset(pair<long double, long double> p1, pair<long double, long double> p2)
{
	if(p1.first >= p2.first && p1.second <= p2.second) 
		return 1 ;
	return 0 ;
}

/// function to copy as described in the paper
vector<stripe> copy1(vector<stripe> S, vector<long double> P, pair<long double, long double> x_int)
{
    vector<stripe> S2 ;

    vector<pair<long double, long double>> part = partition(P) ;
    for(int i = 0; i < part.size(); i++)
    {
        stripe temp ;
        temp.x_int = x_int ;
        temp.x_measure = 0 ;
        temp.y_int = part[i] ;
        S2.push_back(temp) ;
    }

    for(int i = 0; i < S2.size(); i++)
    {
        stripe s2 = S2[i] ;

        for(int j = 0; j < S.size(); j++)
        {
            stripe s = S[j] ;
            if(superset(s.y_int, s2.y_int))
            {
                S2[i].x_measure = S[j].x_measure ;
            }
        }
    }

    return S2 ;
}

/// blacken implementation as shown in the paper
void blacken(vector<stripe> &S, vector<pair<long double, long double>> J)
{
    for(int i = 0; i < S.size(); i++)
    {
        stripe s = S[i] ;
        for(int j = 0; j < J.size(); j++)
        {
            if(subset(s.y_int, J[j]))
            {
                S[i].x_measure = S[i].x_int.second - S[i].x_int.first ;
            }
        }
    }
}

/// concatenates two sets of stripes
vector<stripe> concat(vector<stripe> S1, vector<stripe> S2, vector<long double> P, pair<long double, long double> x_int)
{
    vector<stripe> S ;

    vector<pair<long double, long double>> part = partition(P) ;
    for(int i = 0; i < part.size(); i++)
    {
        stripe temp ;
        temp.x_int = x_int ;
        temp.x_measure = 0 ;
        temp.y_int = part[i] ;
        S.push_back(temp) ;
    }

    for(int i = 0; i < S.size(); i++)
    {
        stripe s1, s2 ;
        for(int j = 0; j < S1.size(); j++)
        {
            if(S1[j].y_int == S[i].y_int)
            {
                s1 = S1[j] ;
                break ;
            }
        }

        for(int j = 0; j < S2.size(); j++)
        {
            if(S2[j].y_int == S[i].y_int)
            {
                s2 = S2[j] ;
                break ;
            }
        }

        S[i].x_measure = s1.x_measure + s2.x_measure ;
    }

    return S ;
}

/// divide and conquer to find stripes
ret_stripe STRIPES(vector<edge> VRX, pair<long double, long double> x_ext)
{
    ret_stripe ret ;

    if(VRX.size() == 1)
    {
        edge v = VRX[0] ;
        if(v.edgetype == 0)
        {
            ret.L.push_back(v.y_int) ;
        }
        else
        {
            ret.R.push_back(v.y_int) ;
        }
        ret.P.push_back(INT_MIN) ;
        ret.P.push_back(v.y_int.first) ;
        ret.P.push_back(v.y_int.second) ;
        ret.P.push_back(INT_MAX) ;

        vector<pair<long double, long double>> part = partition(ret.P) ;
        for(int i = 0 ; i < part.size(); i++)
        {
            stripe temp ;
            temp.x_int = x_ext ;
            temp.y_int = part[i] ;
            temp.x_measure = 0 ;
            ret.S.push_back(temp) ;
        }

        for(int i = 0; i < ret.S.size(); i++)
        {
            stripe s = ret.S[i] ;
            if(s.y_int == v.y_int)
            {
                if(v.edgetype == 0)
                {
                    ret.S[i].x_measure = x_ext.second - v.x ;
                }
                else
                    ret.S[i].x_measure = v.x - x_ext.first ;
            }
        }
    }
    else if(VRX.size() > 1)
    {
        vector<edge> V1, V2 ;
        int half = VRX.size() / 2 ;

        for(int i = 0; i < half; i++)
        {
            V1.push_back(VRX[i]) ;
        }

        for(int i = half; i < VRX.size(); i++)
        {
            V2.push_back(VRX[i]) ;
        }
        long double xm ;
        xm = (VRX[half - 1].x + VRX[half].x) / 2.0 ;

        pair<long double, long double> p1, p2 ;
        p1.first = x_ext.first ;
        p1.second = xm ;
        p2.first = xm ;
        p2.second = x_ext.second ;

        ret_stripe ret1 = STRIPES(V1, p1) ;
        ret_stripe ret2 = STRIPES(V2, p2) ;

        vector<pair<long double, long double>> LR ;

        for(int i = 0; i < ret1.L.size(); i++)
        {
            if(is_present(ret1.L[i], ret2.R) && !is_present(ret1.L[i], LR))
            {
                LR.push_back(ret1.L[i]) ;
            }
        }

        //ret.L
        for(int i = 0; i < ret1.L.size(); i++)
        {
            if(!is_present(ret1.L[i], LR) && !is_present(ret1.L[i], ret.L))
            {
                ret.L.push_back(ret1.L[i]) ;
            }
        }
        for(int i = 0; i < ret2.L.size(); i++)
        {
            if(!is_present(ret2.L[i], ret.L)) 
                ret.L.push_back(ret2.L[i]) ;
        }

        //ret.R
        for(int i = 0; i < ret2.R.size(); i++)
        {
            if(!is_present(ret2.R[i], LR) && !is_present(ret2.R[i], ret.R))
            {
                ret.R.push_back(ret2.R[i]) ;
            }
        }
        for(int i = 0; i < ret1.R.size(); i++)
        {
            if(!is_present(ret1.R[i], ret.R)) 
                ret.R.push_back(ret1.R[i]) ;
        }

        //ret.P
        for(int i = 0; i < ret1.P.size(); i++)
        {
            if(!is_present_int(ret1.P[i], ret.P)) 
                ret.P.push_back(ret1.P[i]) ;
        }
        for(int i = 0; i < ret2.P.size(); i++)
        {
            if(!is_present_int(ret2.P[i], ret.P)) 
                ret.P.push_back(ret2.P[i]) ;
        }

        vector<stripe> S_left = copy1(ret1.S, ret.P, make_pair(x_ext.first, xm)) ;
        vector<stripe> S_right = copy1(ret2.S, ret.P, make_pair(xm, x_ext.second)) ;

        vector<pair<long double, long double>> arg1, arg2 ;
        for(int i = 0; i < ret2.R.size(); i++)
        {
            if(!is_present(ret2.R[i], LR) && !is_present(ret2.R[i], arg1))
            {
                arg1.push_back(ret2.R[i]) ;
            }
        }

        for(int i = 0; i < ret1.L.size(); i++)
        {
            if(!is_present(ret1.L[i], LR) && !is_present(ret1.L[i], arg2))
            {
                arg2.push_back(ret1.L[i]) ;
            }
        }

        blacken(S_left, arg1) ;
        blacken(S_right, arg2) ;

        ret.S = concat(S_left, S_right, ret.P, x_ext) ;
    }
    return ret ;
}

/// driver
int main()
{
    string s1,s2;
    std::cin>>s1>>s2;
    ifstream in(s1);
    ofstream out(s2);
    int n ;
    in >> n ;

    vector<rectangle> rect ;
    long double xmin=INT_MAX;
    long double xmax=INT_MIN;
    for(int i = 0; i < n; i++)
    {
        long double x1, y1, x2, y2 ;
        in >> x1 >> y1 >> x2 >> y2 ;
        xmin=min(xmin,x1);
        xmax=max(xmax,x2);
        rectangle temp ;
        temp.x_int.first = x1 ;
        temp.x_int.second = x2 ;
        temp.y_int.first = y2 ;
        temp.y_int.second = y1 ;

        rect.push_back(temp) ;
    }

    vector<edge> VRX ;
    for(int i = 0 ; i < n; i++)
    {
        edge temp ;
        temp.edgetype = 0 ;
        temp.x = rect[i].x_int.first ;
        temp.y_int = rect[i].y_int ;
        VRX.push_back(temp) ;
        
        temp.edgetype = 1 ;
        temp.x = rect[i].x_int.second ;
        temp.y_int = rect[i].y_int ;
        VRX.push_back(temp) ;
    }

    sort(VRX.begin(), VRX.end(), &comparator_edge) ;
    pair<long double, long double> x_int ;
    x_int.first = xmin;
    x_int.second = xmax;

    ret_stripe ret = STRIPES(VRX, x_int) ;

    long long int sum = 0 ;
    // out << endl << "STRIPES are " << endl ;
    for(auto x:ret.S)
    {
		//cout << x.y_int.first << " " << x.y_int.second << " " << x.x_measure << endl ;
        sum = sum + ( x.x_measure * abs(x.y_int.second - x.y_int.first) ) ;
    }
    // out << endl ;
    out<< sum << endl ;
}