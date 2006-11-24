/**
 * Copyright (c) 2006 Benjamin C. Meyer (ben at meyerhome dot net)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Benjamin Meyer nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <probe.h>
#include <user.h>
#include <qdebug.h>
#include <qvector.h>
/*
# Compute the Pearson correlation coefficient between two arrays of numbers:
#
#  SUM( (x[i] - Xmean)(y[i] - Ymean) ) / SQRT( SUM( (x[i] - Xmean)^2 )(SUM( (y[i] - Ymean)^2 ))   )
#
# It returns values in the range from -1.0 to 1.0
*/
double mean(const QVector<int> &v, int size) {
    int t = 0;
    for (int i = 0; i < size; ++i)
        t += v.at(i);
    return t / (double)v.count();
}

double pearsonCorrelationCoefficient(const QVector<int> &x, const QVector<int> &y, int size) {
    double productOfDeviations = 0;
    double squareOfXDeviations = 0;
    double squareOfYDeviations = 0;
    double xMean = mean(x, size);
    double yMean = mean(y, size);
    for (int i = 0; i < size; ++i) {
        double xValueDeviation = x.at(i) - xMean;
        double yValueDeviation = y.at(i) - yMean;
        productOfDeviations += (xValueDeviation * yValueDeviation);
        squareOfXDeviations += (xValueDeviation * xValueDeviation);
        squareOfYDeviations += (yValueDeviation * yValueDeviation);
    }
    return productOfDeviations / sqrt(squareOfXDeviations * squareOfYDeviations);
}

/**
    One of the basic algorithm is the average algorithm which takes
    the average of all the votes for a movie and guess's that for any user.

*/
class DoubleAverage : public Algorithm {
public:
    DoubleAverage(DataBase *db) : currentMovie(db), user(db), currentMovieAverage(-1) {
        foo.fill(-1, db->totalVotes());
    }
    Algorithm* create() { return new DoubleAverage(currentMovie.dataBase()); };

    void setMovie(int id)
    {
        currentMovie.setId(id);
        currentMovieAverage = 0;
        uint currentMovieVotes = currentMovie.votes();
        for (uint i = 0; i < currentMovieVotes; ++i)
            currentMovieAverage += currentMovie.score(i);
        currentMovieAverage = currentMovieAverage / (double)(currentMovie.votes());
    }

    double determine(int userId)
    {
        user.setId(userId);
        movieUsers.resize(currentMovie.votes());
        User otherUser(currentMovie.dataBase());
        int c = 0;
        uint nextUser = 0;

        uint currentMovieId = currentMovie.id();
        for (int i = 0; i < user.votes(); ++i)
            foo[user.movie(i)] = user.score(i);

        for (uint i = 0; i < currentMovie.votes(); ++i) {
            nextUser = currentMovie.user(i);
            if (nextUser == (uint)userId)
                continue;
            otherUser.setId(nextUser);
            c = 0;

            int userVotes = otherUser.votes();
            if (x.size() < userVotes) {
                x.resize(userVotes);
                y.resize(userVotes);
            }
            for (int j = 0; j < userVotes; ++j) {
                int movie = otherUser.movie(j);
                if (movie == currentMovieId)
                    continue;
                //int r = user.seenMovie(movie);
                int r = foo.at(movie);
                if (r != -1) {
                    x[c] = (r);
                    y[c] = (otherUser.score(j));
                    ++c;
                }
            }
            movieUsers[i] = pearsonCorrelationCoefficient(x, y, c);
        }

        int biggest = 0;
        double largest = -1.0;
        int perfectusers[5] = {0,0,0,0,0};
        double top = 1.1;
        int bounce = 0;
        do {
            bounce = 0;
            top -= 0.1;
            for (int i = 0; i < movieUsers.count(); ++i) {
                int cs = currentMovie.score(i);
                if (movieUsers.at(i) >= top) {
                    perfectusers[cs - 1]++;
                    ++bounce;
                }
                if (largest < movieUsers.at(i)) {
                    largest = movieUsers.at(i);
                    biggest = i;
                }
            }
        } while (bounce <= 5 && top > 0);

        int match = 0;
        for (int i = 0; i < 5; ++i) {
            if (perfectusers[i] > perfectusers[match])
                match = i;
        }
        for (int i = 0; i < user.votes(); ++i)
            foo[user.movie(i)] = -1;


        qDebug() << perfectusers[0] << perfectusers[1] << perfectusers[2] << perfectusers[3] << perfectusers[4] << "pick" << match << "votes:" << currentMovie.votes() << top;
        if (match > -1 && perfectusers[match] != 0) {
            if (perfectusers[match + 1] == perfectusers[match])
                return ((double)(match + 2 + match + 1)/2);
            return (match + 1);

        }
        return currentMovie.score(biggest);
    }

    QVector<int> foo;

    QVector<int> x;
    QVector<int> y;
    QVector<double> movieUsers;

    Movie currentMovie;
    User user;
    double currentMovieAverage;
};
#include <qbitarray.h>
int main(int , char **)
{
    DataBase db;
    db.load();
/*
    User firstUser(&db, 6);

    User user(firstUser);
    Movie movie(&db, 0);
    QBitArray otherUsers(db.totalUsers());
    while (user.id() != db.lastUser()) {
        *//*int c = 0;
        for (int i = 0; i < user.votes(); ++i) {
            movie.setId(user.movie(i));
            for (uint j = 0; j < movie.votes(); ++j) {
                otherUsers[db.mapUser(movie.user(j))] = true;
            }

        }
        //qSort(otherUsers.begin(), otherUsers.end());
        qDebug() << otherUsers.count() << db.totalUsers() << c;
        return 0;
        *//*
        User otherUser(firstUser);
        while (otherUser.id() != db.lastUser()) {
            otherUser.next();
        }
        return 0;
        user.next();
        //qDebug() << user.id();
    }
    qDebug() << user.id();
    return 0;
    /*
    user.setId(userId);
        QVector<double> movieUsers(currentMovie.votes());
        QVector<int> x(32);
        QVector<int> y(32);
        for (uint i = 0; i < currentMovie.votes(); ++i) {
            uint nextUser = currentMovie.user(i);
            if (nextUser == (uint)userId)
                continue;
            User otherUser(currentMovie.dataBase(), currentMovie.user(i));
            int c = 0;
            for (int j = 0; j < user.votes(); ++j) {
                int movie = user.movie(j);
                int r = otherUser.seenMovie(movie);
                if (r != -1) {
                    if (c == x.size()) {
                        x.resize(x.size() * 2 + 1);
                        y.resize(y.size() * 2 + 1);
                    }
                    x[c] = (user.score(j));
                    y[c] = (r);
                    ++c;
                }
            }
            movieUsers[i] = pearsonCorrelationCoefficient(x, y, c);
        }
*/
    
    Probe probe(&db);
    DoubleAverage bf(&db);
    return probe.runProbe(&bf);
}

