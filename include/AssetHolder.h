#ifndef ASSETHOLDER_H_INCLUDED
#define ASSETHOLDER_H_INCLUDED

#include "string"
#include "map"
#include "memory"
#include "iostream"

template <typename Asset>
class AssetHolder {
public:
    bool loadFromFile( const std::string& file, const std::string& id ) {
        std::unique_ptr<Asset> asset( new Asset );

        if( !asset->loadFromFile( file ) ) {
            std::cout << "Unable to find file at: " << file << std::endl;
            return false;
        }

        auto inserted = m_assets.insert( std::make_pair( id, std::move( asset ) ) );

        if( !inserted.second ) {
            std::cout << "Failed inserting asset with ID: " << id << "   At: " << file << std::endl;
            return false;
        }

        return true;
    }

    Asset& getAsset( const std::string& id ) const {
        auto found = m_assets.find( id );

        if( found == m_assets.end() ) {
            std::cout << "Unable to find asset with id: " << id << std::endl;

            found = m_assets.find( "MISSING_TEXTURE" );
        }

        return *found->second;
    }

private:
    std::map<std::string, std::unique_ptr<Asset>> m_assets;
};

#endif // ASSETHOLDER_H_INCLUDED
