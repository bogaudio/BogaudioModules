#!/bin/sh

set -eu

GITHUB_API_URL=https://api.github.com

GITHUB_TOKEN=$1

# Get release url
curl -o release.json \
    --header "Authorization: token ${GITHUB_TOKEN}" \
    --request GET \
    ${GITHUB_API_URL}/repos/${GITHUB_REPOSITORY}/releases/${GITHUB_REF#"refs/"}

UPLOAD_URL=$(jq -r .upload_url release.json)

ASSET_PATH=$(ls dist/*.zip)

curl -i \
    --header "Authorization: token ${GITHUB_TOKEN}" \
    --header "Content-Type: application/zip" \
    --request POST \
    --data-binary @"${ASSET_PATH}" \
    ${UPLOAD_URL%"{?name,label\}"}?name=${ASSET_PATH#"dist/"}
