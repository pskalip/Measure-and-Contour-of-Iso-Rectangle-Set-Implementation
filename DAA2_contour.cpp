#include <bits/stdc++.h>

using namespace std ;


///Rectangle Structure
typedef struct rectangle
{
    pair<double, double> x_int ; //x_interval(bottpm, top)
    pair<double, double> y_int ; //y_interval(bottom, top)
} rectangle ;

///Edge (left or right) Structure
typedef struct edge
{
    pair<double, double> y_int ;
    double x ;
    int edgetype ; // 0 - left, 1 - right
} edge ;

///Edge (bottom or top) Structure
typedef struct edge2
{
    pair<double, double> x_int ;
    double y ;
    int edgetype ; //0 - bottom, 1 - top
} edge2 ;

///Comparator for which edge comes first
bool comparator_edge(const edge& lhs, const edge& rhs) 
{
    if(lhs.x==rhs.x)
        return lhs.edgetype < rhs.edgetype;
   return lhs.x < rhs.x ;
}

///lru type
typedef struct lru
{
    int type ; //left = 0 , right = 1, undef = 2
} lru ;

///Ctree Structure
typedef struct ctree
{
    double x ;
    lru side ;
    struct ctree *lson ;
    struct ctree *rson ;
} ctree ;

///Stripe Structure
typedef struct stripe
{
	pair<double, double> x_int ;
	pair<double, double> y_int ;
	ctree *tree ;
} stripe ;

///Return structure for Stripe function
typedef struct ret_stripe
{
    vector<pair<double, double>> L ;
    vector<pair<double, double>> R ;
    vector<double> P ;
    vector<stripe> S ;
} ret_stripe ;

///Function to partition the set of points
vector<pair<double,double>> partition(vector<double> Y)
{
    sort(Y.begin(), Y.end()) ;
    vector<pair<double,double>> ans ;

    for(int i = 0; i < Y.size() - 1; i++)
    {
        pair<double,double> temp ;
        temp.first = Y[i] ;
        temp.second = Y[i + 1] ;
        ans.push_back(temp) ;
    }
    return ans ;
}

///Function to check if interval is present in a set of intervals
bool is_present(pair<double, double> p, vector<pair<double, double>> vec)
{
    for(int i = 0; i < vec.size(); i++)
    {
        if((p.first == vec[i].first) && (p.second == vec[i].second))
            return true ;
    }
    return false ;
}

///Function to check if point is present in a set of points
bool is_present_int(double p, vector<double> vec)
{
    for(int i = 0; i < vec.size(); i++)
    {
        if(p == vec[i])
            return true ;
    }
    return false ;
}

///Function to check if the first interval is a superset of the second 
int superset(pair<double, double> p1, pair<double, double> p2)
{
	if(p1.first <= p2.first && p1.second >= p2.second) 
		return 1 ;
	return 0 ;
}

///Function to check if the first interval is a subset of the second
int subset(pair<double, double> p1, pair<double, double> p2)
{
	if(p1.first >= p2.first && p1.second <= p2.second) 
		return 1 ;
	return 0 ;
}

///Function COPY as described in the paper
vector<stripe> copy1(vector<stripe> S, vector<double> P, pair<double, double> x_int)
{
    vector<stripe> S2 ;

    vector<pair<double, double>> part = partition(P) ;
    for(int i = 0; i < part.size(); i++)
    {
        stripe temp ;
        temp.x_int = x_int ;
        temp.tree = NULL ;
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
                S2[i].tree = S[j].tree ;
            }
        }
    }

    return S2 ;
}

///Function BlACKEN as described in the paper
void blacken(vector<stripe> &S, vector<pair<double, double>> J)
{
    for(int i = 0; i < S.size(); i++)
    {
        stripe s = S[i] ;
        for(int j = 0; j < J.size(); j++)
        {
            if(subset(s.y_int, J[j]))
            {
                S[i].tree = NULL ;
            }
        }
    }
}

///Comparator to compare ctrees
int comp_ctree(ctree *c1, ctree *c2)
{
    if(c1 == c2)
        return 1 ;
    return 0 ;
}

///Function to concatenate two sets of stripes, given set of points and an interval
vector<stripe> concat(vector<stripe> S1, vector<stripe> S2, vector<double> P, pair<double, double> x_int)
{
    vector<stripe> S ;

    vector<pair<double, double>> part = partition(P) ;
    for(int i = 0; i < part.size(); i++)
    {
        stripe temp ;
        temp.x_int = x_int ;
        temp.tree = NULL ;
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

        ///S[i].x_measure = s1.x_measure + s2.x_measure ;
        if(s1.tree != NULL && s2.tree != NULL)
        {
            S[i].tree = new ctree ;
            S[i].tree -> x = x_int.second ;
            S[i].tree -> side.type = 2 ;
            S[i].tree -> lson = s1.tree ;
            S[i].tree -> rson = s2.tree ;
        }
        else if(s1.tree != NULL && s2.tree == NULL)
        {
            S[i].tree = s1.tree ;
        }
        else if(s1.tree == NULL && s2.tree != NULL)
        {
            S[i].tree = s2.tree ;
        }
        else if(s1.tree == NULL && s2.tree == NULL)
        {
            S[i].tree = NULL ;
        }
    }
    return S ;
}

///Function STRIPES as described in the paper
ret_stripe STRIPES(vector<edge> VRX, pair<double, double> x_ext)
{
    ret_stripe ret ;

    if(VRX.size() == 1)
    {
        //cout << "HERE " << VRX[0].y_int.first << " " << VRX[0].y_int.second << endl ;

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

        vector<pair<double, double>> part = partition(ret.P) ;
        for(int i = 0 ; i < part.size(); i++)
        {
            stripe temp ;
            temp.x_int = x_ext ;
            temp.y_int = part[i] ;
            temp.tree = NULL ;
            ret.S.push_back(temp) ;
        }

        //cout << ret.S.size() << endl ;

        /*for(int i = 0 ; i < ret.S.size(); i++)
        {
            cout << ret.S[i].x_int.first << " " << ret.S[i].x_int.second << " " << ret.S[i].y_int.first << " " << ret.S[i].y_int.second << endl ;
        }*/

        for(int i = 0; i < ret.S.size(); i++)
        {
            stripe s = ret.S[i] ;
            if(s.y_int == v.y_int)
            {
                if(v.edgetype == 0)
                {
                    ret.S[i].tree = new ctree ;
                    ret.S[i].tree -> x = VRX[0].x ;
                    ret.S[i].tree -> side.type = 0 ;
                    ret.S[i].tree -> lson = NULL ;
                    ret.S[i].tree -> rson = NULL ;
                }
                else
                {
                    ret.S[i].tree = new ctree ;
                    ret.S[i].tree -> x = VRX[0].x ;
                    ret.S[i].tree -> side.type = 1 ;
                    ret.S[i].tree -> lson = NULL ;
                    ret.S[i].tree -> rson = NULL ;
                }
            }
        }

        /*cout << ret.S.size() << endl ;

        for(int i = 0 ; i < ret.S.size(); i++)
        {
            cout << ret.S[i].x_int.first << " " << ret.S[i].x_int.second << " " << ret.S[i].y_int.first << " " << ret.S[i].x_int.second << endl ;
        }*/
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
        double xm ;
        xm = (VRX[half - 1].x + VRX[half].x) / 2.0 ;

        pair<double, double> p1, p2 ;
        p1.first = x_ext.first ;
        p1.second = xm ;
        p2.first = xm ;
        p2.second = x_ext.second ;

        ret_stripe ret1 = STRIPES(V1, p1) ;
        ret_stripe ret2 = STRIPES(V2, p2) ;

        vector<pair<double, double>> LR ;

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

        vector<pair<double, double>> arg1, arg2 ;
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

///Function to run DFS on the ctrees to get the x co-ordinates
void dfs(ctree *tree, vector<double> &x)
{
    if(tree == NULL)
        return ;

    if((tree -> lson == NULL) && (tree -> rson == NULL))
    {
        x.push_back(tree -> x) ;
        return ;
    }

    if(tree -> lson != NULL)
        dfs(tree -> lson, x) ;
    if(tree -> rson != NULL)
        dfs(tree -> rson, x) ;
}

///Function to find vertical edges of the rectangles
vector<edge> find_vertical(stripe s)
{
    vector<edge> ans ;

    vector<double> x ;
    dfs(s.tree, x) ;

    for(int i = 0; i < x.size(); i++)
    {
        edge temp ;
        temp.x = x[i] ;
        temp.y_int = s.y_int ;
        ans.push_back(temp) ;
    }
    return ans ;
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
    vector<rectangle> rect, rect2 ;

    for(int i = 0; i < n; i++)
    {
        double x1, y1, x2, y2 ;
        in >> x1 >> y1 >> x2 >> y2 ;
        rectangle temp ;
        temp.x_int.first = x1 ;
        temp.x_int.second = x2 ;
        temp.y_int.first = y2 ;
        temp.y_int.second = y1 ;

        rectangle temp2 ;
        temp2.x_int.first = y2 ;
        temp2.x_int.second = y1 ;
        temp2.y_int.first = x1;
        temp2.y_int.second = x2 ;

        rect.push_back(temp) ;
        rect2.push_back(temp2) ;
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
    pair<double, double> x_int ;
    x_int.first = INT_MIN ;
    x_int.second = INT_MAX ;

    ret_stripe ret = STRIPES(VRX, x_int) ;
    vector<stripe> S = ret.S ;

    vector<edge> vertical_cont ;
    for(int i = 0; i < S.size(); i++)
    {
        vector<edge> temp = find_vertical(S[i]) ;

        for(int j = 0; j < temp.size(); j++)
        {
            vertical_cont.push_back(temp[j]) ;
        }
    }
    
    out << "Vertical lines" << endl ;
    for(int i = 0; i < vertical_cont.size(); i++)
    {
        out << vertical_cont[i].x << ": " << vertical_cont[i].y_int.first << " " << vertical_cont[i].y_int.second << endl ;
    }
    //Interchange axes
    vector<edge> VRX2 ;
    for(int i = 0 ; i < n; i++)
    {
        edge temp ;
        temp.edgetype = 0 ;
        temp.x = rect2[i].x_int.first ;
        temp.y_int = rect2[i].y_int ;
        VRX2.push_back(temp) ;
        
        temp.edgetype = 1 ;
        temp.x = rect2[i].x_int.second ;
        temp.y_int = rect2[i].y_int ;
        VRX2.push_back(temp) ;
    }

    sort(VRX2.begin(), VRX2.end(), &comparator_edge) ;
    pair<double, double> x_int2 ;
    x_int2.first = INT_MIN ;
    x_int2.second = INT_MAX ;

    ret_stripe ret2 = STRIPES(VRX2, x_int2) ;
    vector<stripe> S2 = ret2.S ;

    vector<edge> horizontal_cont ;
    for(int i = 0; i < S2.size(); i++)
    {
        vector<edge> temp = find_vertical(S2[i]) ;

        for(int j = 0; j < temp.size(); j++)
        {
            horizontal_cont.push_back(temp[j]) ;
        }
    }

    out << "Horizontal lines" << endl ;
    for(int i = 0; i < horizontal_cont.size(); i++)
    {
        out << horizontal_cont[i].x << ": " << horizontal_cont[i].y_int.first << " " << horizontal_cont[i].y_int.second << endl ;
    }
}
