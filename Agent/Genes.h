// Genes
// -------------------------------------------------------------------
// Copyright (C) 2009 Anders Nissen
//
// This program is free software; It is covered by the GNU General
// Public License version 2 or any later version.
// See the GNU General Public License for more details (see LICENSE).
//--------------------------------------------------------------------

#ifndef _GENES_H_
#define _GENES_H_

#include <stdio.h>
#include <map>
#include <string>

namespace Motivation {

    class Genes {
    private:
        std::map<std::string, float> gene_map;

    public:
        Genes();
        ~Genes();

        void SetValue(std::string id, float value);
        float GetValue(std::string id);
        float GetInverseValue(std::string id);

        Genes* Combine(Genes* g);
        Genes* Crossover(Genes* g);
        Genes* Mutate(float rate = 0.2);

        Genes* GetRandomGenes();

        float GetAverage();
        int GetGeneCount();

        void Print();

    private:

    };

}

#endif //_GENES_H_
